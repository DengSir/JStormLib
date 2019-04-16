{
  "targets": [
    {
      "target_name": "StormLib",
      "sources": [
        "src/Entry.cc",
        "src/openArchive.cc",
        "src/CreateError.cc",
        "src/StormArchive.cc"
      ],
      "dependencies": ["stormlib.gyp:StormLibRAS"],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
