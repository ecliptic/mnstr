module Text = {
  [@bs.val] external parseInt_ : (string, int) => int = "parseInt";
  /** parseInt with an assumed radix of 10 */
  let parseInt = (string) => parseInt_(string, 10);
};

module Promise = {
  let (resolve, reject) = Js.Promise.(resolve, reject);
  [@bs.new] external makeError : string => exn = "Error";
  let cancelTimeout = (delay: int, message: string) =>
    Js.Promise.make(
      (~resolve as _, ~reject) =>
        Js.Global.setTimeout(() => [@bs] reject(makeError(message)), delay) |> ignore
    );
  let timeout = (~timeout=10000, promises) => {
    let timeout = cancelTimeout(timeout, "DataProvider failed to initialize.");
    Js.Promise.(race([|timeout, all(promises)|]))
  };
  let rejectWith = (message) => reject(makeError(message));
  let getOrReject = (opt) =>
    switch opt {
    | Some(value) => resolve(value)
    | None => rejectWith("Unable to get")
    };
  /**
   * Reject a promise with the given error if the results are an empty array
   */
  let rejectIfEmpty = (~error, results) =>
    switch results {
    | [||] => reject(makeError(error))
    | results => resolve(results)
    };
  /* Pick the first result in a Promise that resolved with an array */
  let pickFirst = (results) => resolve(results[0]);
  /**
   * Infix operators for the Promise type courtesy of BsAbstract
   */
  module Infix = {
    include BsAbstract.Promise.Infix;
    let (<!>) = (a, b) => Js.Promise.catch(b, a);
  };
};

module Env = {
  let isBrowser: bool =
    [%raw {|typeof window !== 'undefined' && typeof window.document !== 'undefined'|}]
    |> Js.to_bool;
  let isNode: bool =
    [%raw {|typeof module !== 'undefined' && typeof module.exports !== 'undefined'|}] |> Js.to_bool;
};

module Http = {
  let errorToJson = (exn: exn) : Js.Json.t => {
    let str = Js.Json.stringifyAny(exn);
    /* parseExn should be safe because the string came from stringifyAny */
    switch str {
    | Some(str) => str |> Js.Json.parseExn
    | None => Js.Json.null
    }
  };
  /**
   * A basic heartbeat route that always returns 200 "ok"
   */
  let heartbeat = (_, res: Express.Response.t) => {
    res |> Express.Response.sendString("ok") |> ignore;
    Js.Promise.resolve(Js.undefined)
  };
  /**
   * Express error handling middleware
   */
  let handleErrors = (~isDev) =>
    Express.(
      Middleware.fromError(
        (_next, err, _req, res) => {
          let error = errorToJson(err);
          /* json dict built via mutation */
          let json = Js.Dict.empty();
          Js.Dict.set(json, "success", Js.Json.boolean(Js.false_));
          if (isDev) {
            Js.Dict.set(json, "error", error)
          };
          let response = Js.Json.object_(json);
          res
          |> Response.status(Response.StatusCode.InternalServerError)
          |> Response.sendJson(response)
        }
      )
    );
};

module Database = {
  let primaryUuid = (knex, table, column) => {
    let colName = column |> Js.Nullable.toOption |> Js.Option.getWithDefault("id");
    table##uuid(colName)##primary()##defaultTo(knex##raw("uuid_generate_v4()"))##comment(
      "The uuid primary key."
    )
  };
  let foreignUuid =
      (
        table,
        column: string,
        reference: {. "table": string, "column": string},
        required: Js.boolean
      ) => {
    let col = table##uuid(column);
    if (Js.to_bool(required)) {
      col##notNullable()
    };
    table##foreign(column)##references(reference##column)##inTable(reference##table);
    col
  };
};
