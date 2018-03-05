module Express = {
  module Cors = {
    [@bs.module] external make : 'config => Express.Middleware.t = "cors";
  };
  module Morgan = {
    [@bs.module] external make : string => Express.Middleware.t = "morgan";
  };
  module BodyParser = {
    module Json = {
      [@bs.module "body-parser"] external make : 'config => Express.Middleware.t = "json";
    };
  };
  module GraphQl = {
    let graphiql = ApolloServerExpress.createGraphiQLExpressMiddleware("/graphql");
  };
};
