type session = {
  .
  "accessToken": Js.Nullable.t(string),
  "idToken": Js.Nullable.t(string),
  "expiresAt": Js.Nullable.t(string),
  "scope": Js.Nullable.t(string)
};

module Keys = {
  let prefix = "mnstr";
  let accessToken = prefix ++ ":access_token";
  let idToken = prefix ++ ":id_token";
  let expiresAt = prefix ++ ":expires_at";
  let scope = prefix ++ ":scope";
};

let get = (): session => Dom.Storage.({
  "accessToken": localStorage |> getItem(Keys.accessToken) |> Js.Nullable.fromOption,
  "idToken": localStorage |> getItem(Keys.idToken) |> Js.Nullable.fromOption,
  "expiresAt": localStorage |> getItem(Keys.expiresAt) |> Js.Nullable.fromOption,
  "scope": localStorage |> getItem(Keys.scope) |> Js.Nullable.fromOption
});

let clear = () => {
  open Dom.Storage;
  localStorage |> removeItem(Keys.accessToken);
  localStorage |> removeItem(Keys.idToken);
  localStorage |> removeItem(Keys.expiresAt);
  localStorage |> removeItem(Keys.scope);
};

let logout = clear;
