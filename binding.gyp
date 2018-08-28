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
      "libraries": [ "<(module_root_dir)/StormLib/bin/StormLibRAS.lib" ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
