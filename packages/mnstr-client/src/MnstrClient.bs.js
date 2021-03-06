// Generated by BUCKLESCRIPT VERSION 2.2.2, PLEASE EDIT WITH CARE
'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var Js_option = require("bs-platform/lib/js/js_option.js");
var MnstrUtils = require("@ecliptic/mnstr-core/src/MnstrUtils.bs.js");
var ApolloLinks = require("reason-apollo/src/ApolloLinks.bs.js");
var ApolloLink = require("apollo-link");
var GraphqlTag = require("graphql-tag");
var Js_primitive = require("bs-platform/lib/js/js_primitive.js");
var ReasonApollo = require("reason-apollo/src/ReasonApollo.bs.js");
var Js_null_undefined = require("bs-platform/lib/js/js_null_undefined.js");
var Option$BsAbstract = require("bs-abstract/src/implementations/Option.bs.js");
var ApolloLinkError = require("apollo-link-error");
var MnstrClientBrowser = require("./MnstrClientBrowser.bs.js");
var ApolloInMemoryCache = require("reason-apollo/src/ApolloInMemoryCache.bs.js");

var Browser = /* module */[/* Session */MnstrClientBrowser.Session];

function assign(prim, prim$1) {
  return Object.assign(prim, prim$1);
}

function empty() {
  return { };
}

function make(uri, fetch, _) {
  return ApolloLinks.createHttpLink(uri, /* None */0, fetch, /* None */0, /* None */0, /* None */0, /* () */0);
}

var HttpLink = /* module */[/* make */make];

function withAuthHeader(headers, token) {
  return {
          headers: Object.assign(headers, {
                Authorization: "Bearer " + (String(token) + "")
              })
        };
}

function make$1(getSession) {
  return ApolloLinks.createContextLink((function () {
                var headers = {
                  Authorization: ""
                };
                var session = Curry._1(getSession, /* () */0);
                return Js_option.getWithDefault({
                            headers: headers
                          }, Curry._2(Option$BsAbstract.Infix[/* <#> */4], Js_primitive.null_undefined_to_opt(session.accessToken), (function (param) {
                                  return withAuthHeader(headers, param);
                                })));
              }));
}

var AuthLink = /* module */[
  /* withAuthHeader */withAuthHeader,
  /* make */make$1
];

function toOption(prim) {
  if (prim == null) {
    return /* None */0;
  } else {
    return [prim];
  }
}

function make$2() {
  return ApolloLinkError.onError((function (errors) {
                var graphQLErrors = errors.graphQLErrors;
                var networkError = errors.networkError;
                if (!(graphQLErrors == null)) {
                  graphQLErrors.forEach((function (error) {
                          var message = error.message;
                          var locations = JSON.stringify(error.locations);
                          var path = error.path;
                          console.log("[GraphQL Error]: Message: " + (String(message) + (", Location: " + (String(locations === undefined ? /* None */0 : [locations]) + (", Path: " + (String(path) + ""))))));
                          return /* () */0;
                        }));
                }
                if (networkError == null) {
                  return /* () */0;
                } else {
                  var code = Js_option.getWithDefault("", Js_primitive.null_undefined_to_opt(networkError.result.error.code));
                  if (code === "invalid_token") {
                    console.log("[Authentication error]: Clearing session.");
                    return MnstrClientBrowser.Session[/* logout */3](/* () */0);
                  } else {
                    console.log("[Network error]: " + (String(networkError) + ""));
                    return /* () */0;
                  }
                }
              }));
}

var ErrorLink = /* module */[
  /* toOption */toOption,
  /* getWithDefault */Js_option.getWithDefault,
  /* make */make$2
];

function make$3() {
  return ApolloInMemoryCache.createInMemoryCache(/* Some */[(function (obj) {
                  return obj.id;
                })], /* None */0, /* () */0);
}

var Cache = /* module */[/* make */make$3];

function asQueryString(query) {
  return GraphqlTag(query);
}

function parseResponse(parse, response) {
  return Object.assign({ }, Object.assign(response, {
                  parsed: Curry._1(parse, response.data)
                }));
}

function query(fetchPolicy, request, client) {
  var query$1 = request.query;
  var partial_arg = request.parse;
  return Curry._2(MnstrUtils.Promise[/* Infix */8][/* <#> */1], client.query({
                  query: GraphqlTag(query$1),
                  variables: request.variables,
                  fetchPolicy: Js_null_undefined.fromOption(fetchPolicy)
                }), (function (param) {
                return parseResponse(partial_arg, param);
              }));
}

function mutate(fetchPolicy, request, client) {
  var query = request.query;
  var partial_arg = request.parse;
  return Curry._2(MnstrUtils.Promise[/* Infix */8][/* <#> */1], client.mutate({
                  mutation: GraphqlTag(query),
                  variables: request.variables,
                  fetchPolicy: Js_null_undefined.fromOption(fetchPolicy)
                }), (function (param) {
                return parseResponse(partial_arg, param);
              }));
}

function make$4(fetch, getSession, uri, connectToDevTools, _) {
  var links = /* array */[
      make$2(/* () */0),
      make(uri, fetch, /* () */0)
    ].concat(getSession ? /* array */[make$1(getSession[0])] : /* array */[]);
  var match = MnstrUtils.Env[/* isBrowser */0];
  return ReasonApollo.createApolloClient(make$3(/* () */0), ApolloLink.from(links), /* Some */[match !== 0 ? false : true], /* None */0, connectToDevTools, /* None */0, /* () */0);
}

exports.Browser = Browser;
exports.assign = assign;
exports.empty = empty;
exports.HttpLink = HttpLink;
exports.AuthLink = AuthLink;
exports.ErrorLink = ErrorLink;
exports.Cache = Cache;
exports.asQueryString = asQueryString;
exports.parseResponse = parseResponse;
exports.query = query;
exports.mutate = mutate;
exports.make = make$4;
/* MnstrUtils Not a pure module */
