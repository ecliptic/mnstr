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
  let pickFirst = (results) => resolve(results[0]);
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
