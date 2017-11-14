$(document).ready(function () {
  var refresh = setInterval(function () {
    $.ajax({
      url: window.location.href + 'status',
      success: function (e) {
        var resp = JSON.parse(e);
        console.log(resp);
        console.log(resp.status);
        if (resp.status == 'finished') {
          carregar_pagina();
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

  function carregar_pagina(){
    $.ajax({
      url: window.location.href,
      success: function (data) {
        document.write(data);
      },
      error: function (data) {

      },
      type: 'POST',
      cache: false,
      contentType: false,
      processData: false
    });
  }
});