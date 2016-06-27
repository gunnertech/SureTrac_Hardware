require('dotenv').config();

var spawn = require('child_process').spawn;
var gps = spawn('cat', ['/dev/ttyACM0']);
var needle = require('needle');


gps.stdout.on('data', function (data) {
  sentence = String(data);

  if (sentence.substring(0, 6) == "$GPRMC") {
    throttle(function() {
      needle.put(process.env.URL, { "nmea-sentence": sentence }, function(err, resp) {
        if (err) {
          console.log("Error: ", err);
        }
      });
    }, process.env.RATE)();
  }
});

gps.stderr.on('data', function (data) {
  console.log('stderr: ' + data);
});

gps.on('exit', function (code) {
  console.log('child process exited with code ' + code);
  gps = spawn('cat', ['/dev/ttyACM0']);
});

// Stolen from https://jsfiddle.net/jonathansampson/m7G64/
function throttle (callback, limit) {
  var wait = false;              // Initially, we're not waiting
  return function () {           // We return a throttled function
    if (!wait) {                 // If we're not waiting
      callback.call();           // Execute users function
      wait = true;               // Prevent future invocations
      setTimeout(function () {   // After a period of time
        wait = false;            // And allow future invocations
      }, limit);
    }
  }
}
