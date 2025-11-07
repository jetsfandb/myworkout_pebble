// A public URL to your configuration HTML file. 
// You can use a service like GitHub Pages or Dropbox.
// var configUrl = 'https://jetsfandb.github.io/myworkout_pebble/config.html';

var settings = {};
var dict = {};

function loadSettings() {
  // Load any existing settings from localStorage if needed
  // This function can be expanded based on specific requirements
  console.log('Loading existing settings (if any)');

}

function saveSettings() {
  // Save settings to localStorage if needed
  // This function can be expanded based on specific requirements
  console.log('Saving settings (if needed)'); 
}

  Pebble.sendAppMessage(dict,
    function() { console.log('[JS] Settings update message sent successfully'); },
    function(e) { console.log('[JS] Settings update message failed: ' + JSON.stringify(e)); }
  );

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
  loadSettings();
});


// Listen for when the configuration page is requested
Pebble.addEventListener('showConfiguration', function() {
  console.log('Showing configuration page');
  loadSettings();
    var configHtml = `
<!DOCTYPE html>
<html>
<head>
  <title>Workout Config</title>
  <meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: sans-serif; padding: 10px; }
    h2 { margin: 10px 0; }
    input[type="text"], input[type="number"] { width: 100%; box-sizing: border-box; }
    .form-row { margin-bottom: 10px; }
    .exercise-item, .setting-item { border: 1px solid #ccc; padding: 10px; margin-bottom: 10px; }
    .exercise-actions, .setting-actions { text-align: right; }
    .delete-btn { color: red; }
    .settings-list { padding-left: 20px; }
  </style>
</head>
<body>
  <h1>Workout Config</h1>

  <form id="configForm">
    <div class="form-row">
      <label for="workoutName">Workout Name:</label>
      <input type="text" id="workoutName" placeholder="Enter workout name">
      <label for="workoutRest">Workout Rest (seconds):</label>
      <input type="number" id="workoutRest" placeholder="Enter workout rest">
    </div>

    <div id="exerciseList"></div>
    <button type="button" id="addExerciseBtn">Add Exercise</button>

    <div class="form-row">
      <button type="submit">Save</button>
    </div>
  </form>

  <script>
    document.addEventListener('DOMContentLoaded', function() {
      // Load and apply settings from localStorage
      var config = JSON.parse(localStorage.getItem('workoutConfig')) || {
        workoutName: '',
        workoutRest: 0,
        exercises: []
      };
      
      var form = document.getElementById('configForm');
      var exerciseList = document.getElementById('exerciseList');

      document.getElementById('workoutName').value = config.workoutName;

      // Function to render the full exercise list
      function renderExercises() {
        exerciseList.innerHTML = '';
        config.exercises.forEach(function(exercise, exerciseIndex) {
          var exerciseItem = document.createElement('div');
          exerciseItem.className = 'exercise-item';
          exerciseItem.innerHTML = ``
            <h2>Exercise ${exerciseIndex + 1}</h2>
            <div class="form-row">
              <label>Title:</label>
              <input type="text" data-field="title" value="${exercise.title}">
            </div>
            <div class="form-row">
              <label>Weight:</label>
              <input type="number" data-field="weight" value="${exercise.weight}">
            </div>
            <div class="form-row">
              <label>Count:</label>
              <input type="number" data-field="count" value="${exercise.count}">
            </div>
            <h3>Settings</h3>
            <div class="settings-list" data-exercise-index="${exerciseIndex}"></div>
            <button type="button" class="add-setting-btn" data-exercise-index="${exerciseIndex}">Add Setting</button>
            <div class="exercise-actions">
              <button type="button" class="delete-btn" data-exercise-index="${exerciseIndex}">Delete Exercise</button>
            </div>
          ``;
          exerciseList.appendChild(exerciseItem);

          var settingsList = exerciseItem.querySelector('.settings-list');
          exercise.settings.forEach(function(setting, settingIndex) {
            var settingItem = document.createElement('div');
            settingItem.className = 'setting-item';
            settingItem.innerHTML = ``
              <div class="form-row">
                <label>Title:</label>
                <input type="text" data-field="title" value="${setting.title}">
              </div>
              <div class="form-row">
                <label>Value:</label>
                <input type="number" data-field="value" value="${setting.value}">
              </div>
              <div class="setting-actions">
                <button type="button" class="delete-setting-btn" data-exercise-index="${exerciseIndex}" data-setting-index="${settingIndex}">Delete Setting</button>
              </div>
            ``;
            settingsList.appendChild(settingItem);
          });
        });
      }

      // Add a new exercise
      document.getElementById('addExerciseBtn').addEventListener('click', function() {
        config.exercises.push({
          title: '',
          weight: 0,
          count: 0,
          settings: []
        });
        renderExercises();
      });

      // Handle events for dynamically created elements
      exerciseList.addEventListener('click', function(event) {
        // Delete exercise
        if (event.target.classList.contains('delete-btn')) {
          var index = parseInt(event.target.dataset.exerciseIndex);
          config.exercises.splice(index, 1);
          renderExercises();
        }
        // Add setting
        if (event.target.classList.contains('add-setting-btn')) {
          var index = parseInt(event.target.dataset.exerciseIndex);
          config.exercises[index].settings.push({ title: '', value: 0 });
          renderExercises();
        }
        // Delete setting
        if (event.target.classList.contains('delete-setting-btn')) {
          var exerciseIndex = parseInt(event.target.dataset.exerciseIndex);
          var settingIndex = parseInt(event.target.dataset.settingIndex);
          config.exercises[exerciseIndex].settings.splice(settingIndex, 1);
          renderExercises();
        }
      });
      
      // Update config object on input change
      exerciseList.addEventListener('input', function(event) {
        var input = event.target;
        var exerciseItem = input.closest('.exercise-item');
        var settingItem = input.closest('.setting-item');
        
        if (settingItem) {
          // Update setting value
          var exerciseIndex = parseInt(settingItem.closest('.settings-list').dataset.exerciseIndex);
          var settingIndex = Array.from(settingItem.parentNode.children).indexOf(settingItem);
          var field = input.dataset.field;
          var value = (field === 'value') ? parseInt(input.value) : input.value;
          config.exercises[exerciseIndex].settings[settingIndex][field] = value;
        } else if (exerciseItem) {
          // Update exercise value
          var exerciseIndex = Array.from(exerciseList.children).indexOf(exerciseItem);
          var field = input.dataset.field;
          var value = (field === 'weight' || field === 'count') ? parseInt(input.value) : input.value;
          config.exercises[exerciseIndex][field] = value;
        }
      });
      
      document.getElementById('workoutName').addEventListener('input', function(event) {
        config.workoutName = event.target.value;
      });

      document.getElementById('workoutRest').addEventListener('input', function(event) {
        config.workoutRest = parseInt(event.target.value) || 0;
      });
      
      // Handle form submission
      document.getElementById('configForm').addEventListener('submit', function(event) {
        event.preventDefault();
        document.location = 'pebblejs://close#' + encodeURIComponent(JSON.stringify(config));
      });

      function getQueryParam(name) {
        name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
        var regex = new RegExp("[\\?&]" + name + "=([^&#]*)");
        var results = regex.exec(location.search);
        return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
      }

      renderExercises();
    });
  </script>
</body>
</html>
`;
  var configUrl = 'data:text/html;charset=utf-8,' + encodeURIComponent(configHtml);
  Pebble.openURL(configUrl);
});



// Listen for when the configuration page is closed
Pebble.addEventListener('webviewclosed', function(e) {
  try {
  console.log('webviewclosed');
  var configData;
  if (e.response) {
    // Decode the data from the HTML form
    configData = JSON.parse(decodeURIComponent(e.response));
    console.log('Received config data: ' + JSON.stringify(configData));

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

    saveSettings();
    // 3. Send the flattened dictionary to the watch
    Pebble.sendAppMessage(dict, function(e) {
      console.log('Config data sent successfully!');
    }, function(e) {
      console.log('Error sending config data: ' + JSON.stringify(e));
    });
    
  } else {
    console.log('Configuration cancelled.');
  }
} catch (err) {
  console.log('Error processing configuration data: ' + err);
}
});

