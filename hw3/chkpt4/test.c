void codegen();

void codegen() {
  int data[5];
  int *p = data;
  data[2] = 1;
  delay(data);
  delay(p);
  delay(data[2]);
  p = p + 2;
  *p = 2;
  delay(p);
  delay(*p);
  delay(data[2]);
  p = data;
  *(p+3) = 5;
  delay(data[3]);
}
