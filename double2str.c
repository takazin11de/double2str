#include <stdio.h>

#define FLOAT10_DIGIT (17)
#define FLOAT10_MAX_DIGIT (FLOAT10_DIGIT+11+12+1)
#define EX_BITS (11)
#define EX_BITS1 (EX_BITS+1)

typedef struct _Float10{
  int digit;
  int is_zero;
  int is_inf;
  int sign;
  char data[FLOAT10_MAX_DIGIT];
} Float10;

void Float10_Init(Float10 *_this){
  _this->digit=0;
  int i;
  _this->is_zero=1;
  _this->is_inf=0;
  _this->sign=0;
  for(i=0; i<FLOAT10_MAX_DIGIT; i++){
    _this->data[i]=0;
  }
}
void Float10_Imm(Float10 *_this,unsigned int _a){
  unsigned int a1,a2,ans,i,j;
  a1=_a;
  a2=0;
  
  Float10_Init(_this);
  if(_a!=0){
    _this->is_zero=0;
  }
  for(i=0;;i++){
    if(a1==0){
      break;
    }
    for(j=FLOAT10_MAX_DIGIT-1; 0!=j; j--){
      _this->data[j]=_this->data[j-1];
    }
    a2=a1/10;
    ans=a1-a2*10;
    _this->data[0]=ans;
    _this->digit++;
    a1=a2;
  }
}
void Float10_Move(Float10 *_this, Float10 *_a){ // this = a
  Float10 *a;
  int i;

  for(i=0; i<FLOAT10_MAX_DIGIT; i++){
    _this->data[i]=_a->data[i];
  }
  _this->digit=_a->digit;
  _this->is_zero=_a->sign;
  _this->sign=_a->sign;
}

void Float10_Add(Float10 *_this, Float10 *_a, Float10 *_b){ // this = a + b
  Float10 *a,*b;
  int i,ib;
  unsigned int b_tmp;
  Float10 ans;
  Float10_Init(&ans);
  int carry_flag=0;

  if(_a->is_zero && _b->is_zero){
    Float10_Imm(_this,0);
    return;
  }
  if(_a->is_zero){
    Float10_Move(_this, _b);
    return;
  }
  if(_b->is_zero){
    Float10_Move(_this, _a);
    return;
  }

  if(_a->digit > _b->digit){
    a=_a;
    b=_b;
  }else{
    a=_b;
    b=_a;
  }

  for(i=FLOAT10_MAX_DIGIT-1; 0<=i; i--){
      ib=i-(a->digit - b->digit);
      if(0<=ib & ib<FLOAT10_MAX_DIGIT){
        b_tmp = b->data[ib];
      }else{
        b_tmp = 0;
      }
      ans.data[i] = a->data[i] + b_tmp + carry_flag;
      if(9<ans.data[i]){
        ans.data[i] -= 10;
        carry_flag=1;
      }else{
        carry_flag=0;
      }
  }
  if(carry_flag){
    for(i=FLOAT10_MAX_DIGIT-1; 0!=i; i--){
      ans.data[i]=ans.data[i-1];
    }
    ans.data[0]=1;
  }

  for(i=0; i<FLOAT10_MAX_DIGIT; i++){
    _this->data[i]=ans.data[i];
  }
  _this->digit=a->digit + carry_flag;

  _this->is_zero=1;
  for(i=0; i<FLOAT10_MAX_DIGIT; i++){
    if(_this->data[i]!=0){
      _this->is_zero=0;
      break;
    }
  }
  if(_this->is_zero){
    _this->digit=1;
  }
}

void Float10_Mul(Float10 *_this, Float10 *_a, Float10 *_b){ // this = a * b
  char data[FLOAT10_MAX_DIGIT*2-1];
  int i,j,k;
  unsigned int data_tmp;
  int carry_flag=0;
  if(_a->is_zero || _b->is_zero){
    for(i=0; i<FLOAT10_MAX_DIGIT; i++){
      _this->data[i]=0;
    }
    _this->digit=1;
    _this->is_zero=1;
    return;
  }
  for(i=0; i<FLOAT10_MAX_DIGIT*2-2; i++){
    data[i]=0;
  }
  for(i=FLOAT10_MAX_DIGIT-1; 0<=i; i--){
    data_tmp=0;
    for(j=FLOAT10_MAX_DIGIT-1; 0<=j; j--){
      data[i+j] += _a->data[i] * _b->data[j] + data_tmp;
      data_tmp = data[i+j]/10;
      data[i+j] = data[i+j] - data_tmp * 10;
    }
    if(i!=0){
      data[i-1]+=data_tmp;
    }
  }
  if(data_tmp){
    for(i=FLOAT10_MAX_DIGIT-1; 0!=i; i--){
      data[i]=data[i-1];
    }
    data[0]=data_tmp;
    carry_flag=1;
      
  }
  for(i=0; i<FLOAT10_MAX_DIGIT; i++){
    _this->data[i]=data[i];
  }
  _this->digit=_a->digit + _b->digit + carry_flag-1;
  _this->is_zero=0;
}

void Float10_Print(Float10 *_this){
  int i;
  Float10 a;
  if(_this->sign){
    printf("-");
  }
  if(_this->is_inf){
    printf("inf\n");
    return;
  }
  Float10_Init(&a);
  if(_this->data[FLOAT10_DIGIT]>4){
    Float10_Imm(&a,1);
    a.digit=_this->digit-FLOAT10_DIGIT+1;
  }
  Float10_Add(&a,&a,_this);
  for(i=0; i<FLOAT10_DIGIT; i++){
    printf("%u",a.data[i]);
    if(i==0){printf(".");}
  }
  printf("e%+03d\n", a.digit-1+_this->is_zero);
}

void Float10_PrintP(Float10 *_this){
  int i;
  Float10 a;
  Float10_Init(&a);
  Float10_Add(&a,&a,_this);
  for(i=0; i<FLOAT10_MAX_DIGIT; i++){
    printf("%u",a.data[i]);
    if(i==0){printf(".");}
  }
  printf("e%d\n", a.digit-1);
  printf("is_zero:%d\n", _this->is_zero);
  printf("is_inf:%d\n", _this->is_inf);
  printf("sing:%d\n", _this->sign);
}

typedef struct _MyDouble{
  unsigned int c;
  unsigned int d;
  unsigned long long e;
} MyDouble;

void MyDouble_Init(MyDouble *_this, double x){
  unsigned long long b;
  (*(double*)&b)=x;

  _this->c=((b>>52)&0x7FF);
  _this->d=b>>63;
  _this->e=b&0xFFFFFFFFFFFFF;
/*
  printf("x %lf\n",x);
  printf("b %llX\n",b);
  printf("c %d\n",_this->c);
  printf("d %X\n",_this->d);
  printf("e %llX\n",_this->e);
  */
}

void Float10_Double(Float10 *_this, double x){
  Float10 ex1[EX_BITS1];
  Float10 ex2[EX_BITS1];
  Float10 data1,data2,float10_1;
  Float10_Init(&data1);
  Float10_Init(&data2);
  Float10_Init(&float10_1);
  Float10_Imm(&float10_1,1);
  MyDouble f;
  int i,j;
  int ii,ii2;
  int f_ii;

  MyDouble_Init(&f, x);
  Float10_Init(_this);
  Float10_Imm(_this,0);

  for(i=0;i<EX_BITS1;i++){
    Float10_Init(&ex1[i]);
  }
  Float10_Imm(&ex1[0],5);
  ex1[0].digit-=1;
  for(i=1;i<EX_BITS1;i++){
    Float10_Mul(&ex1[i],&ex1[i-1],&ex1[i-1]);
  }
  for(i=0;i<EX_BITS1;i++){
    Float10_Init(&ex2[i]);
  }
  Float10_Imm(&ex2[0],2);
  for(i=1;i<EX_BITS1;i++){
    Float10_Mul(&ex2[i],&ex2[i-1],&ex2[i-1]);
  }
  
  ii=f.c-52;

  _this->sign=f.d;

  for(i=0;i<52+1;i++){
    if((f.e>>i)&1 || i==52){
      Float10_Imm(&data1,0);
      f_ii=0;
      if(ii==2047){
        //無限大
        _this->is_inf=1;
        break;
      }else if(ii==1023){
        Float10_Imm(&data1,1);
      }else if(ii>1023){
        ii2=ii-1023;
        for(j=0;j<EX_BITS1;j++){
          if((ii2>>j)&1){
            if(f_ii){
              Float10_Mul(&data1,&data1,&ex2[j]);
            }
            else{
              Float10_Mul(&data1,&float10_1,&ex2[j]);
              f_ii=1;
            }
          }
        }
      }else if(ii<1023){
        ii2=-ii+1023;
        if(f.c==0){
          ii2--;
          if(i==52){
            //非正規数
            break;
          }
        }
        for(j=0;j<EX_BITS1;j++){
          if((ii2>>j)&1){
            if(f_ii){
              Float10_Mul(&data1,&data1,&ex1[j]);
            }
            else{
              Float10_Mul(&data1,&float10_1,&ex1[j]);
              f_ii=1;
            }
          }
        }
      }
      Float10_Add(_this,_this,&data1);
    }
    ii++;
  }

}
struct MyFloat64{
  unsigned long long mantissa:52;
  unsigned long long exponent:11;
  unsigned long long sign:1;
};
union AA{
  struct MyFloat64 ff;
  unsigned long long l;
  double f;
};

int main(){
  Float10 a,b,c;

  union AA aa;
  
  Float10_Init(&a);

  Float10_Double(&a, 3.141592);
  Float10_Print(&a);
  printf("%.16e\n\n", 3.141592);

  Float10_Double(&a, 1.2345e+5);
  Float10_Print(&a);
  printf("%.16e\n\n", 1.2345e+5);

  Float10_Double(&a, 1.2345e-5);
  Float10_Print(&a);
  printf("%.16e\n\n", 1.2345e-5);

  Float10_Double(&a, 1.2345e+50);
  Float10_Print(&a);
  printf("%.16e\n\n", 1.2345e+50);

   Float10_Double(&a, 1.2345e-50);
  Float10_Print(&a);
  printf("%.16e\n\n", 1.2345e-50);

  Float10_Double(&a, 1.2345e+100);
  Float10_Print(&a);
  printf("%.16e\n\n", 1.2345e+100);

  Float10_Double(&a, 1.2345e-100);
  Float10_Print(&a);
  printf("%.16e\n\n", 1.2345e-100);

  aa.ff.sign=0;//最大値
  aa.ff.exponent=0x7FE;
  aa.ff.mantissa=0xFFFFFFFFFFFFF;
  Float10_Double(&a, aa.f);
  Float10_Print(&a);
  printf("%.16e\n\n", aa.f);


  Float10_Double(&a, 2.2250738585072014e-308);
  Float10_Print(&a);
  printf("%.16e\n\n", 2.2250738585072014e-308);

  Float10_Double(&a, 2.2250738585072014e-308);
  Float10_Print(&a);
  printf("%.16e\n\n", 2.2250738585072014e-308);

  Float10_Double(&a, 3.337610787760802e-308);
  Float10_Print(&a);
  printf("%.16e\n\n", 3.337610787760802e-308);

  Float10_Double(&a, 5.562684646268003e-309);
  Float10_Print(&a);
  printf("%.16e\n\n", 5.562684646268003e-309);
  
  Float10_Double(&a, 5.562684646268003e-309);
  Float10_Print(&a);
  printf("%.16e\n\n", 5.562684646268003e-309);

  Float10_Double(&a, 1.0430033711752506e-308);
  Float10_Print(&a);
  printf("%.16e\n\n", 1.0430033711752506e-308);

  aa.ff.sign=0;//非正規化数
  aa.ff.exponent=0;
  aa.ff.mantissa=0x8000000000000;
  Float10_Double(&a, aa.f);
  Float10_Print(&a);
  printf("%.16e\n\n", aa.f);

  aa.ff.sign=0;//非正規化数
  aa.ff.exponent=0;
  aa.ff.mantissa=0x0000000000001;
  Float10_Double(&a, aa.f);
  Float10_Print(&a);
  printf("%.16e\n\n", aa.f);

  aa.ff.sign=0;//非正規化数
  aa.ff.exponent=0;
  aa.ff.mantissa=0x0000000000002;
  Float10_Double(&a, aa.f);
  Float10_Print(&a);
  printf("%.16e\n\n", aa.f);

  aa.ff.sign=0;//+0
  aa.ff.exponent=0;
  aa.ff.mantissa=0x0000000000000;
  Float10_Double(&a, aa.f);
  Float10_Print(&a);
  printf("%.16e\n\n", aa.f);

  aa.ff.sign=0;//-0
  aa.ff.exponent=0;
  aa.ff.mantissa=0x0000000000000;
  Float10_Double(&a, aa.f);
  Float10_Print(&a);
  printf("%.16e\n\n", aa.f);


  Float10_Double(&a, 1/0.0);//inf
  Float10_Print(&a);
  printf("%.16e\n\n", 1/0.0);

  Float10_Double(&a, -1/0.0);//-inf
  Float10_Print(&a);
  printf("%.16e\n\n", -1/0.0);
  
  return 0;
}