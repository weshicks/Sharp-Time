(function() {
  loadOptions();
  submitHandler();
})();

function submitHandler() {
  var $submitButton = $('#submitButton');

  $submitButton.on('click', function() {
    console.log('Submit');

    var return_to = getQueryParam('return_to', 'pebblejs://close#');
    document.location = return_to + encodeURIComponent(JSON.stringify(getAndStoreConfigData()));
  });
}

function loadOptions() {
  var $backgroundColorPicker = $('#backgroundColorPicker');
  var $timeColorPicker = $('#timeColorPicker');
  var $dateColorPicker = $('#dateColorPicker');
  var $infoColorPicker = $('#infoColorPicker');

  if (localStorage.backgroundColor) {
    $backgroundColorPicker[0].value = localStorage.backgroundColor;
    $timeColorPicker[0].value = localStorage.timeColor;
    $dateColorPicker[0].value = localStorage.dateColor;
    $infoColorPicker[0].value = localStorage.infoColor;
    $enableWeather.prop('checked', localStorage.enableWeather);
  }
}

function getAndStoreConfigData() {
  var $backgroundColorPicker = $('#backgroundColorPicker');
  var $timeColorPicker = $('#timeColorPicker');
  var $dateColorPicker = $('#dateColorPicker');
  var $infoColorPicker = $('#infoColorPicker');
  var $enableWeather   = $('#enableWeather');

  var options = {
    backgroundColor: $backgroundColorPicker.val(),
    timeColor: $timeColorPicker.val(),
    dateColor: $dateColorPicker.val(),
    infoColor: $infoColorPicker.val(),
    enableWeather: $enableWeather.is(':checked')
  };

  localStorage.backgroundColor = options.backgroundColor;
  localStorage.timeColor = options.timeColor;
  localStorage.dateColor = options.dateColor;
  localStorage.infoColor = options.infoColor;
  localStorage.enableWeather = options.enableWeather;

  console.log('Got options: ' + JSON.stringify(options));
  return options;
}

function getQueryParam(variable, defaultValue) {
  var query = location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i++) {
    var pair = vars[i].split('=');
    if (pair[0] === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return defaultValue || false;
}