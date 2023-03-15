{
  "targets": [
    {
      "target_name": "zkAppUtilsJS",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "./src/zkAppUtilsClassJS.cpp",
        "/usr/include/zymkey/zk_app_utils.h"
        "/usr/include/zymkey/zkAppUtilsClass.h"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "/usr/include/zymkey"
      ],
      "libraries": [
        "/usr/lib/libzkAppUtilsClassCPP.so",
        "/usr/lib/libzk_app_utils.so",
        "/usr/lib/libzk.so"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}
