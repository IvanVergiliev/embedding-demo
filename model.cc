#include <node.h>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <stack>

using namespace v8;
using namespace std;

const long long max_size = 2000;         // max length of strings
const long long N = 40;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries

const int DEFAULT_SIZE = 200;

struct Vector {
  vector<float> values;
  Vector(int length): values(length) {}

  float& operator[](int index) {
    return values[index];
  }

  const float& operator[](int index) const {
    return values[index];
  }

  void normalize() {
    float length = 0;
    for (auto f: values) {
      length += f * f;
    }
    length = sqrt(length);
    for (auto& f: values) {
      f /= length;
    }
  }

  void print() const {
    for (auto f: values) {
      printf("%f ", f);
    }
    printf("\n");
  }

  static float cosineDist(const Vector& a, const Vector& b) {
    float res = 0;
    for (int i = 0; i < a.values.size(); ++i) {
      res += a[i] * b[i];
    }
    return res;
  }

  Vector& operator-=(const Vector& rhs) {
    for (int i = 0; i < rhs.values.size(); ++i) {
      values[i] -= rhs[i];
    }
    return *this;
  }

  Vector& operator+=(const Vector& rhs) {
    for (int i = 0; i < rhs.values.size(); ++i) {
      values[i] += rhs[i];
    }
    return *this;
  }
};

bool calc(const vector<int>& input, Vector& result);
vector<string> findClosest(const Vector& v);

long long a;
long long b;
long long words;
long long size;
float len;
float* M;
char* vocab;
vector<Vector> repr;
map<string, int> wordIndex;

int loadData(const string& filename) {
  FILE* f = fopen(filename.c_str(), "rb");
  if (f == NULL) {
    printf("Input file not found\n");
    return -1;
  }
  fscanf(f, "%lld", &words);
  fscanf(f, "%lld", &size);
  vocab = (char *)malloc((long long)words * max_w * sizeof(char));
  repr.resize(words, Vector(size));
  for (b = 0; b < words; b++) {
    a = 0;
    while (1) {
      vocab[b * max_w + a] = fgetc(f);
      if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
      if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
    }
    vocab[b * max_w + a] = 0;
    wordIndex[string(&vocab[b * max_w])] = b;
    for (a = 0; a < size; a++) {
      fread(&repr[b][a], sizeof(float), 1, f);
    }
    repr[b].normalize();
  }
  fclose(f);
//  repr[1000].print();
//  printf("%d\n", wordIndex["море"]);
  Vector res(DEFAULT_SIZE);
  vector<int> input;
  input.push_back(12);
  input.push_back(26);
  input.push_back(6);
//  {12, 26, 6};
  bool ok = calc(input, res);
  printf("ok: %d\n", ok);
  vector<string> best = findClosest(res);
//  printf("%s\n", best[0].c_str());
//  printf("%s\n", best[1].c_str());
//  printf("%s\n", best[2].c_str());
//  printf("%s\n", best[3].c_str());
  return 0;
}

vector<string> query(const vector<int>& expression) {
  Vector res(DEFAULT_SIZE);
  bool ok = calc(expression, res);
  if (!ok) {
    return vector<string>();
  }
  return findClosest(res);
}

vector<string> findClosest(const Vector& v) {
  vector<pair<float, int>> dist;
  for (int i = 0; i < words; ++i) {
    dist.push_back(make_pair(Vector::cosineDist(v, repr[i]), i));
  }
  sort(dist.begin(), dist.end());
  reverse(dist.begin(), dist.end());
  vector<string> res; //(dist.begin(), dist.begin() + 10);
  for (int i = 0; i < 10; ++i) {
    res.push_back(&vocab[dist[i].second * max_w]);
  }
  return res;
}

string letter[] = {"а", "б", "в", "г", "д", "е", "ж", "з", "и", "й", "к", "л", "м",
  "н", "о", "п", "р", "с", "т", "у", "ф", "х", "ц", "ч", "ш", "щ", "ъ", "ь", "ю",
  "я", "А", "Б", "В", "Г", "Д", "Е", "Ж", "З", "И", "Й", "К", "Л", "М", "Н", "О",
  "П", "Р", "С", "Т", "У", "Ф", "Х", "Ц", "Ч", "Ш", "Щ", "Ъ", "Ь", "Ю", "Я"};

string toWord(const vector<int> letterPositions) {
  string res;
  for (int pos: letterPositions) {
//    printf("%d %s\n", pos, letter[pos].c_str());
    res += letter[pos];
  }
  return res;
}

bool calc(const vector<int>& input, Vector& result) {
  vector<int> current_word;
  vector<vector<int>> words;
  vector<int> ops;
  for (int c: input) {
//    printf("c: %d\n", c);
    if (0 <= c && c < 60) {
      current_word.push_back(c);
    } else if (c < 0) {
      words.push_back(current_word);
      current_word.clear();
      ops.push_back(c);
    }
  }
//  printf("1\n");
  if (current_word.size()) {
    words.push_back(current_word);
  }
//  printf("2\n");
  if (ops.size() + 1 != words.size()) {
    printf("ops: %d, words: %d\n", ops.size(), words.size());
    return false;
  }
//  printf("3\n");
  if (!wordIndex.count(toWord(words[0]))) {
    printf("Couldn't find word %s\n", toWord(words[0]).c_str());
    return false;
  }
//  printf("4\n");
  result = repr[wordIndex[toWord(words[0])]];
//  printf("word: %s\n", toWord(words[0]).c_str());
//  printf("word index: %d\n", wordIndex[toWord(words[0])]);
  int index = 1;
  for (int op: ops) {
//    printf("op: %d, index: %d\n", op, index);
    string curWord = toWord(words[index]);
    if (!wordIndex.count(curWord)) {
      return false;
    }
    Vector cur = repr[wordIndex[curWord]];
    if (op == -1) {
      result -= cur;
    } else if (op == -2) {
      result += cur;
    }
    ++index;
  }
  result.normalize();
  return true;
}

void Load(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  String::Utf8Value filename(args[0]->ToString());
  std::string s(*filename);
  loadData(s);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, s.c_str()));
}

void Query(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  Handle<Array> arr = Handle<Array>::Cast(args[0]);
  vector<int> v;
  for (int i = 0; i < arr->Length(); ++i) {
    v.push_back(arr->Get(i)->Int32Value());
  }
  vector<string> res = query(v);
  string s;
  for (int i = 0; i < res.size(); ++i) {
    if (i != 0) {
      s += ',';
    }
    s += res[i];
  }
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, s.c_str()));
}

void Method(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, "world"));
}

void init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "hello", Method);
  NODE_SET_METHOD(exports, "load", Load);
  NODE_SET_METHOD(exports, "query", Query);
}

NODE_MODULE(addon, init)
