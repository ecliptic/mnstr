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
