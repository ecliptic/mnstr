// Generated by BUCKLESCRIPT VERSION 2.2.2, PLEASE EDIT WITH CARE
'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var Promise$BsAbstract = require("bs-abstract/src/implementations/Promise.bs.js");

function resolve(prim) {
  return Promise.resolve(prim);
}

function reject(prim) {
  return Promise.reject(prim);
}

function cancelTimeout(delay, message) {
  return new Promise((function (_, reject) {
                setTimeout((function () {
                        return reject(new Error(message));
                      }), delay);
                return /* () */0;
              }));
}

function timeout($staropt$star, promises) {
  var timeout$1 = $staropt$star ? $staropt$star[0] : 10000;
  var timeout$2 = cancelTimeout(timeout$1, "DataProvider failed to initialize.");
  return Promise.race(/* array */[
              timeout$2,
              Promise.all(promises)
            ]);
}

function rejectWith(message) {
  return Promise.reject(new Error(message));
}

function getOrReject(opt) {
  if (opt) {
    return Promise.resolve(opt[0]);
  } else {
    return Promise.reject(new Error("Unable to get"));
  }
}

function rejectIfEmpty(error, results) {
  if (results.length !== 0) {
    return Promise.resolve(results);
  } else {
    return Promise.reject(new Error(error));
  }
}

function pickFirst(results) {
  return Promise.resolve(Caml_array.caml_array_get(results, 0));
}

function $less$bang$great(a, b) {
  return a.catch(Curry.__1(b));
}

var Infix_000 = /* <$> */Promise$BsAbstract.Infix[0];

var Infix_001 = /* <#> */Promise$BsAbstract.Infix[1];

var Infix_002 = /* <*> */Promise$BsAbstract.Infix[2];

var Infix_003 = /* >>= */Promise$BsAbstract.Infix[3];

var Infix_004 = /* =<< */Promise$BsAbstract.Infix[4];

var Infix = /* module */[
  Infix_000,
  Infix_001,
  Infix_002,
  Infix_003,
  Infix_004,
  /* <!> */$less$bang$great
];

var Promise$1 = /* module */[
  /* resolve */resolve,
  /* reject */reject,
  /* cancelTimeout */cancelTimeout,
  /* timeout */timeout,
  /* rejectWith */rejectWith,
  /* getOrReject */getOrReject,
  /* rejectIfEmpty */rejectIfEmpty,
  /* pickFirst */pickFirst,
  /* Infix */Infix
];

var isBrowser = +(typeof window !== 'undefined' && typeof window.document !== 'undefined');

var isNode = +(typeof module !== 'undefined' && typeof module.exports !== 'undefined');

var Env = /* module */[
  /* isBrowser */isBrowser,
  /* isNode */isNode
];

exports.Promise = Promise$1;
exports.Env = Env;
/* isBrowser Not a pure module */
