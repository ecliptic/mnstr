module Router = {
  include MnstrServerRouter;
};

module Http = {
  open MnstrAdapters.Express;
  module App = Express.App;
  module Middleware = Express.Middleware;
  module Response = Express.Response;
  type middleware =
    | NoOp
    | Use(Middleware.t)
    | UseOnPath(string, Middleware.t);
  let make = (~middleware=?, ~routes, ~bodyLimit="100kb", ~port=?, ~isDev=false, ~onListen=?, ()) => {
    let app = App.make();
    let default = [
      Use(isDev ? Morgan.make("dev") : Morgan.make("combined")),
      /* Parse json */
      Use(BodyParser.Json.make({"limit": bodyLimit})),
      /* If this is dev, attach the GraphiQL middleware */
      isDev ? UseOnPath("/graphiql", GraphQl.graphiql) : NoOp,
      /* Handle plain REST requests */
      Use(MnstrServerRouter.Web.make(~routes, ())),
      /* Handle errors */
      Use(MnstrUtils.Http.handleErrors(~isDev))
    ];
    (
      switch middleware {
      | Some(transform) => transform(default)
      | None => default
      }
    )
    |> List.iter(
         (maybe) =>
           switch maybe {
           | Use(ware) => App.use(app, ware)
           | UseOnPath(path, ware) => App.useOnPath(app, ~path, ware)
           | NoOp => ()
           }
       );
    /* Start taking requests */
    App.listen(app, ~port?, ~onListen?, ())
  };
};
