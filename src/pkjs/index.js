// A public URL to your configuration HTML file. 
// You can use a service like GitHub Pages or Dropbox.
var configUrl = 'https://github.com/jetsfandb/myworkout_pebble/blob/main/src/pkjs/config.html';

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
});

// Listen for when the configuration page is requested
Pebble.addEventListener('showConfiguration', function() {
  console.log('Showing configuration page');
  Pebble.openURL(configUrl);
});

// Listen for when the configuration page is closed
Pebble.addEventListener('webviewclosed', function(e) {
  var configData;
  if (e.response) {
    // Decode the data from the HTML form
    configData = JSON.parse(decodeURIComponent(e.response));
    console.log('Received config data: ' + JSON.stringify(configData));

    var dict = {};
    var exerciseIndex = 0;
    var settingIndex = 0;
    
    // 1. Send the workout name
    if (configData.workoutName) {
      dict.WorkoutName = configData.workoutName;
    }
    
    // 2. Flatten the exercise array
    if (configData.exercises && Array.isArray(configData.exercises)) {
      dict.ExerciseCount = configData.exercises.length;
      
      configData.exercises.forEach(function(exercise, i) {
        if (i >= 10) return; // Hard limit to avoid exceeding buffer

        // Exercise keys
        dict['ExerciseTitle_' + i] = exercise.title;
        dict['ExerciseWeight_' + i] = exercise.weight;
        dict['ExerciseCount_' + i] = exercise.count;
        
        // Flatten the settings array for this exercise
        if (exercise.settings && Array.isArray(exercise.settings)) {
          dict['ExerciseSettingsCount_' + i] = exercise.settings.length;
          
          exercise.settings.forEach(function(setting, j) {
            if (j >= 5) return; // Hard limit for settings
            
            // Setting keys
            dict['SettingTitle_' + i + '_' + j] = setting.title;
            dict['SettingValue_' + i + '_' + j] = setting.value;
          });
        } else {
          dict['ExerciseSettingsCount_' + i] = 0;
        }
      });
    } else {
      dict.ExerciseCount = 0;
    }

    // 3. Send the flattened dictionary to the watch
    Pebble.sendAppMessage(dict, function(e) {
      console.log('Config data sent successfully!');
    }, function(e) {
      console.log('Error sending config data: ' + JSON.stringify(e));
    });
    
  } else {
    console.log('Configuration cancelled.');
  }
});

