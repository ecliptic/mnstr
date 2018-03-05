module Session = {
  open Dom.Storage;
  /**
   * A session with authentication data
   */
  type session = {
    .
    "accessToken": Js.Nullable.t(string),
    "idToken": Js.Nullable.t(string),
    "expiresAt": Js.Nullable.t(string),
    "scope": Js.Nullable.t(string)
  };
  /**
   * LocalStorage keys
   */
  module Keys = {
    let prefix = "mnstr";
    let accessToken = prefix ++ ":access_token";
    let idToken = prefix ++ ":id_token";
    let expiresAt = prefix ++ ":expires_at";
    let scope = prefix ++ ":scope";
  };
  /**
   * Retrieve the session from localStorage
   */
  let get = () : session =>
      {
        "accessToken": localStorage |> getItem(Keys.accessToken) |> Js.Nullable.fromOption,
        "idToken": localStorage |> getItem(Keys.idToken) |> Js.Nullable.fromOption,
        "expiresAt": localStorage |> getItem(Keys.expiresAt) |> Js.Nullable.fromOption,
        "scope": localStorage |> getItem(Keys.scope) |> Js.Nullable.fromOption
      }
    ;
  /**
   * Clear the session in localStorage
   */
  let clear = () => {
    localStorage |> removeItem(Keys.accessToken);
    localStorage |> removeItem(Keys.idToken);
    localStorage |> removeItem(Keys.expiresAt);
    localStorage |> removeItem(Keys.scope)
  };
  let logout = clear;
};
