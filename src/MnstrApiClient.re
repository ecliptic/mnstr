open BsAbstract;

[@bs.module "isomorphic-fetch"] external isomorphicFetch : 'polyfill = "default";

[@bs.module "apollo-link-error"] external onError : 'a => ReasonApolloTypes.apolloLink = "";

[@bs.val] external fetch_ : ApolloClient.fetch = "fetch";

[@bs.module] external gql : ReasonApolloTypes.gql = "graphql-tag";

let (assign, empty) = Js.Obj.(assign, empty);

/**
 * The response returned by the GraphQL server
 */
type queryResult = {
  .
  "data": Js.Json.t,
  "errors": Js.Nullable.t(array(Js.Exn.t)),
  "loading": Js.boolean,
  "networkStatus": int,
  "stale": Js.boolean
};

/**
 * A Query request object to be passed to Apollo
 */
type queryObj = {
  .
  "query": ReasonApolloTypes.queryString,
  "variables": Js.Json.t,
  "fetchPolicy": Js.Nullable.t(string)
};

/**
 * A Mutation request object to be passed to Apollo
 */
type mutationObj = {
  .
  "mutation": ReasonApolloTypes.queryString,
  "variables": Js.Json.t,
  "fetchPolicy": Js.Nullable.t(string)
};

type request('response) = {
  .
  "parse": Js.Json.t => 'response, "query": string, "variables": Js.Json.t
};

/**
 * The Apollo client returned from ReasonApollo.createApolloClient below
 */
type apolloClientJs = {
  .
  "query": [@bs.meth] (queryObj => Js.Promise.t(queryResult)),
  "mutate": [@bs.meth] (mutationObj => Js.Promise.t(queryResult))
};

module HttpLink = {
  type t = ReasonApolloTypes.apolloLink;
  /**
     * If this is in-browser, supply fetch from the window object
     */
  let fetch = MnstrUtils.Env.isBrowser ? Some(fetch_) : isomorphicFetch;
  let make = (~uri) : t => ApolloLinks.createHttpLink(~uri, ~fetch?, ());
};

/**
 * A Link that puts authorization data into the headers
 */
module AuthLink = {
  type t = ReasonApolloTypes.apolloLink;
  /**
   * Add the Authorization header to the given headers object with the given auth token
   */
  let withAuthHeader = (headers, token) => {
    "headers": Js.Nullable.return(Js.Obj.assign(headers, {"Authorization": {j|Bearer $token|j}}))
  };
  let make = (~getSession) : t =>
    ApolloLinks.createContextLink(
      () => {
        let return = Js.Nullable.return;
        let getWithDefault = Js.Option.getWithDefault;
        let headers = {"Authorization": ""};
        let getAccessToken = (session) => session##accessToken |> Js.Nullable.toOption;
        Option.Infix.(
          getSession()
          |> getAccessToken
          <#> withAuthHeader(headers)
          |> getWithDefault({"headers": return(headers)})
        )
      }
    );
};

/**
 * An error link that listens for authentication errors and handles them by clearing the session
 */
module ErrorLink = {
  type t = ReasonApolloTypes.apolloLink;
  let toOption = Js.Nullable.toOption;
  let getWithDefault = Js.Option.getWithDefault;
  let make = () : t =>
    onError(
      (errors) => {
        let (graphQLErrors, networkError) = (errors##graphQLErrors, errors##networkError);
        switch (graphQLErrors |> toOption) {
        | Some(errors) =>
          errors
          |> Js.Array.forEach(
               (error) => {
                 let (message, locations, path) = (
                   error##message,
                   error##locations |> Js.Json.stringifyAny,
                   error##path
                 );
                 Js.log(
                   {j|[GraphQL Error]: Message: $message, Location: $locations, Path: $path|j}
                 )
               }
             )
        | None => ()
        };
        switch (networkError |> toOption) {
        | Some(error) =>
          let code = error##result##error##code |> toOption |> getWithDefault("");
          if (code === "invalid_token") {
            Js.log("[Authentication error]: Clearing session.");
            MnstrBrowserSession.logout()
          } else {
            Js.log({j|[Network error]: $error|j})
          }
        | None => ()
        }
      }
    );
};

/**
 * The standard Apollo in-memory cache
 */
module Cache = {
  let make = () => ApolloInMemoryCache.createInMemoryCache(~dataIdFromObject=(obj) => obj##id, ());
};

/**
 * Magic to use the underlying JS client directly using the type above
 */
external asApolloClient : ApolloClient.generatedApolloClient => apolloClientJs =
  "%identity";

/**
 * Use `gql` to convert a string to a parsed query
 */
let asQueryString = (query: string) => [@bs] gql(query);

/**
 * A standard HttpLink pointing to the given endpoint
 */
let parseResponse = (parse, response) =>
  {"parsed": parse(response##data)} |> assign(response) |> assign(empty());

/* A helper function to use the Apollo client to issue queries */
let query = (~fetchPolicy=?, ~request, client) =>
  MnstrUtils.Promise.Infix.(
    client##query({
      "query": asQueryString(request##query),
      "variables": request##variables,
      "fetchPolicy": fetchPolicy |> Js.Nullable.fromOption
    })
    <#> parseResponse(request##parse)
  );

/* A helper function to use the Apollo client to issue mutations */
let mutate = (~fetchPolicy=?, ~request, client) =>
  MnstrUtils.Promise.Infix.(
    client##mutate({
      "mutation": asQueryString(request##query),
      "variables": request##variables,
      "fetchPolicy": fetchPolicy |> Js.Nullable.fromOption
    })
    <#> parseResponse(request##parse)
  );

/**
 * Initialize a new ApiClient
 */
let make = (~uri, ~getSession=?, ~connectToDevTools, ()) => {
  /* By default, every client gets an ErrorLink and an HttpLink */
  let links =
    [|ErrorLink.make(), HttpLink.make(~uri)|]
    |> Js.Array.concat(
         switch getSession {
         /* If the getSession function was provided, add the AuthLink */
         | Some(getSession) => [|AuthLink.make(~getSession)|]
         | None => [||]
         }
       );
  /* The initialize the client with cache, links, and config */
  ReasonApollo.createApolloClient(
    ~cache=Cache.make(),
    ~link=ApolloLinks.from(links),
    ~ssrMode=MnstrUtils.Env.isBrowser ? Js.false_ : Js.true_,
    ~connectToDevTools,
    ()
  )
  |> asApolloClient
};
