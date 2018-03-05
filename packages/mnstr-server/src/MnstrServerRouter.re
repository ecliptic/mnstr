let resolve = Js.Promise.resolve;

module Web = {
  external asMiddleware : PromiseRouter.t => Express.Middleware.t = "%identity";
  type routes =
    | Get(string, PromiseRouter.promiseMiddleware)
    | Post(string, PromiseRouter.promiseMiddleware);
  let make = (~routes: option(list(routes))=?, ()) => {
    let toMount = routes |> Js.Option.getWithDefault([Get("/", MnstrServerUtils.Http.heartbeat)]);
    let router = PromiseRouter.make();
    toMount |> List.iter((route) => switch route {
    | Get(path, route) => PromiseRouter.get(router, ~path, route)
    | Post(path, route) => PromiseRouter.post(router, ~path, route)
    });
    asMiddleware(router)
  };
};
