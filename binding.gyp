{
  "targets": [
    {
      "target_name": "regkey",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "./src/Binding.cpp",
        "./src/RegKey.cpp",
        "./src/RegKeyWrap.cpp"
       ],
      "include_dirs": [
        "./include",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "defines": [ 
        "NODE_ADDON_API_CPP_EXCEPTIONS"
      ]
    }
  ]
}
