$(document).ready(function () {
  var vectorize = function () {
    var expr = $('#expression').val();
    console.log(expr);
    $.get('/vectorize', {expression: expr}, function (data) {
      console.log(data);
      var html = '<div>Най-близки думи:</div><ol>';
      data.words.forEach(function (word) {
        html += '<li>' + word + '</li>';
      });
      html += '</ol>';
      $('#results').html(html);
    });
  };
  $('#vectorize').click(vectorize);
  $('#vectorize').on('touchend', vectorize);
  $('#expression').keydown(function (event) {
    if (event.keyCode == 13) {
      vectorize();
      return false;
    }
  })
});
