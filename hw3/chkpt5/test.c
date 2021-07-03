void codegen();
int modulo(const int *a, const int *b);
int gcd(const int a, const int b);

void codegen()
{
  // int a = gcd(12, 18); // a = 6
  // int b = gcd(8, 9); // b = 1
  int a = 1;
  int b = 2;
  delay(modulo(&a,&b));
  // delay(a);
  // delay(b);
  // const int c6 = 6, c4 = 4;
  // digitalWrite(26 + modulo(&c6, &c4), HIGH);
  // delay(a * 1000);
  // digitalWrite(26 + modulo(&c6, &c4), LOW);
  // delay(b * 1000);
}

int modulo(const int *a, const int *b)
{
  delay(55555);
  delay(*a);
  delay(*b);
  delay(55555);
  if (*a < *b) {
    return *a;
  }

  int ret = *a;
  while (1) {
    ret = ret - *b;
    if (ret < *b) {
      break;
    }
  }
  return ret;
}

int gcd(const int a, const int b)
{
  delay(222222222222);
  delay(a);
  delay(b);
  delay(222222222222);
  if (b != 0) {
    return gcd(b, modulo(&a, &b));
  } else {
    return a;
  }
}

