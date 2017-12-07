$(document).ready(function () {
  var refresh = setInterval(function () {
    $.ajax({
      url: window.location.href.substring(0, window.location.href.lastIndexOf('/')) + '/status',
      success: function (e) {
        var resp = JSON.parse(e);
        console.log(resp);
        console.log(resp.status);
        if (resp.status == 'finished') {
          window.location.href = window.location.href.substring(0, window.location.href.lastIndexOf('/'))
          clearInterval(refresh);
        }
      },
      error: function (e) {

      },
      type: 'GET',
      cache: false,
      contentType: false,
      processData: false
    });
  }, 1000);
});