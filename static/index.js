$(document).ready(function () {
  var vectorize = function (event) {
    var expr = $('#expression').val();
    console.log(expr);
    $.get('/vectorize', {expression: expr}, function (data) {
      console.log(data);
      var html = '';
      data.words.forEach(function (word) {
        html += '<div>' + word + '</div>';
      });
      $('#results').html(html);
    });
  };
  $('#vectorize').click(vectorize);
  $('#vectorize').on('touchend', vectorize);
});
