{
  "targets": [
    {
      "target_name": "rf24",
      "sources": [ "rf24-wrapper.cc" ],
      "include_dirs": [ "RF24/librf24-rpi/librf24" ],
      "library_dirs": [ "RF24/librf24-rpi/librf24" ],
      "libraries": [ "-lrf24" ]
    }
  ]
}
