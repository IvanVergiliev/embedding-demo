var express = require('express');
var app = express();

var addon = require('./build/Release/model');

var alphabet = 'абвгдежзийклмнопрстуфхцчшщъьюяАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЮЯ';

var toArray = function (str) {
  var res = [];
  for (var i = 0; i < str.length; ++i) {
    if (str[i] == '-') {
      res.push(-1);
    } else if (str[i] == '+') {
      res.push(-2);
    } else if ('а' <= str[i] && str[i] <= 'я') {
      res.push(alphabet.indexOf(str[i]));
    } else if ('А' <= str[i] && str[i] <= 'Я') {
      res.push(alphabet.indexOf(str[i]));
    }
  }
  return res;
}

addon.load('/Users/ivan/src/word2vec/bg.bin');
console.log('loaded data');
console.log(addon.query([12, 26, 6]));
console.log(toArray('мъж'));
console.log(toArray('крал-мъж+жена'));
console.log(addon.query(toArray('крал-мъж+жена')));
console.log(addon.query(toArray('София-Париж+България')));
console.log(addon.query(toArray('Париж - София + България')));
var str = 'Париж - София + България';
var res = addon.query(toArray(str));
res = res.split(',').filter(function (value) {
  return str.indexOf(value) == -1;
});
console.log(res);

var query = function (str) {
  var res = addon.query(toArray(str));
  res = res.split(',').filter(function (value) {
    return str.indexOf(value) == -1;
  });
  return res;
};

console.log(query('крал - мъж'));
console.log(query('Франция - Париж + Лондон'));
console.log(query('жена - жени + Лондон'));
console.log(query('десет + десет'));
console.log(query('десет'));
console.log(query('сто + крак'));

app.get('/', function (req, res) {
  res.send(addon.hello());
});

app.get('/vectorize', function (req, res) {
  var expr = req.query.expression;
  console.log(expr);
  var words = query(expr);
  res.json({words: words});
});

app.use(express.static('static'));

var server = app.listen(9000, function (err) {
  if (err) {
    console.err(err);
  }
  var host = server.address().address;
  var port = server.address().port;
  console.log('listening on ' + host + ':' + port);
});
