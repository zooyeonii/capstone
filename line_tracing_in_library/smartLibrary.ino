// Copyright 2019. LeeSeungWoo All Right Reserved.
// File Name : smartLibrary.ino
// Auther : Lee Seung Woo (201320038)
// E-mail Address : rmawpwk@ajou.ac.kr
// Rescription : Project 이미지센싱을 통한 서가정보 확인 시스템
// Team Name : 사서고생하지마(이승우/송승근/김태완/조성제/김주연)
// Last Changed : 2019.12.20

#include <Timer.h>
#include <AccelStepper.h>
#include <Button.h>
Timer t;


///////////////nfc
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#define PN532_SCK  35//주황
#define PN532_MOSI 33//파랑
#define PN532_SS   31//노랑
#define PN532_MISO 29 //초록
#define nfc_tag1 176
#define nfc_tag2 171
#define nfc_tag3 166
#define nfc_tag4 162
#define nfc_tag5 157
#define nfc_master 245 // 파란색 플라스틱 칩 아이디 245
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

///////////////////스텝모터 1,2,3 정의
#define step_pin1 22 //left바퀴
#define dir_pin1 23  //left바퀴
#define en_pin1  24 //카메라모터
#define step_pin2 10 //right바퀴
#define dir_pin2 11  //right바퀴
#define en_pin2  47 //카메라모터
long motorPosition1 = 0;
long motorPosition2 = 0;
#define en_pin3  46 //카메라모터
#define step_pin3  44 //카메라모터
#define dir_pin3   42 //카메라모터
long motorPosition3 = 0;
bool oldMoveCaseTemp1 = 0;
bool oldMoveCaseTemp2 = 0;
AccelStepper stepper1(AccelStepper::FULL2WIRE, step_pin1, dir_pin1);
AccelStepper stepper2(AccelStepper::FULL2WIRE, step_pin2, dir_pin2);
AccelStepper stepper3(AccelStepper::FULL2WIRE, step_pin3, dir_pin3);
bool NewCaseStart2 = 0;
long taketimefor1kan = 35500; //서가 한칸당 카메라가 움직이는데 필요한 정도
long devideCamera = 2;

#define SensorL 49    //왼쪽 적외선센서
#define SensorC 47    //중앙 적외선센서  > 1이면 라인을 밟은것
#define SensorR 45    //오른쪽 적외선센서
#define SensorBL 40    //뒤왼쪽 적외선센서
#define SensorBC 38    //뒤중앙 적외선센서  > 1이면 라인을 밟은것
#define SensorBR 36    //뒤오른쪽 적외선센서

#define Light 13    //전구센서
#define light1 48
#define light2 51
#define light3 50
#define light11 24  //전구센서 적외선 l
#define light22 26  //전구센서 적외선 c
#define light33 28  //전구센서 적외선 r
#define butttton 39
char  button, old_button;
char seg[3] = {light1, light2, light3};
char seg2[3] = {light11, light22, light33};
char num[4][3] = {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {0, 0, 0}}; //조명 번호
char num2[5][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 1}, {0, 0, 0}}; //끝날때 조명 신호

int temp = 0;
int temp1 = 0;
int r, l, c ;
int br, bl, bc ;
int oldR, oldL, oldC ;
int oldBr, oldBL, oldBc;
bool firstSeogaCheck = -1; //첫번째 서가를 위해 카메라 올렸는지 아닌지
bool NewCaseStart; //케이스가 바뀐경우 카메라의 위치조절을 위한 toggle변수
unsigned long time_start, time_passed; //5초에 한번씩 사진찍으라는 명령을 시리얼입력을 통해 라즈베리파이로 보내기 위해 시간을 셈

struct seoga {
  bool done;
  int level;      // 서가의 층
  int filledLevel;// 차 있는 서가의 층
  int startCard;  // 서가의 좌측 NFC카드 번호
  int endCard;    // 서가의 우측 NFC카드 번호
};
#define TOTALSEOGA 2
seoga SEOGA[TOTALSEOGA];

int currentSeoga = -1;
int beforeSeogaLevel = -1;
int countOfDoneLevel = -1 ;
int currentNFC ;    // 현재 NFC카드 번호

#define FinalCard 171
#define AdminCard 194
#define CameraCard3 220 //up 150000
#define CameraCard4 223 //down 150000
#define CameraCard5 245 //up 300000
#define CameraCard6 253 //down 300000

bool directionOfCar = 1;

#define MODE_GO 1111
#define MODE_SHOT_GO 2222
#define MODE_SHOT_BACK 3333
#define MODE_STOP 4444
int controlMode = MODE_STOP ; // 1111 = 정주행, 2222 = 스캔+정주행, 3333 = 스캔+역주행, 4444 = 종료
int oldControlMode = controlMode;

long time_start2 = 0;
long time_passed2 = 0;
long time_passed3 = 0;
long time_start3 = 0;
#define GO 111
#define TURN_RIGHT 222
#define TURN_LEFT 333
#define BACK 444
#define BACK_TURN_RIGHT 555
#define BACK_TURN_LEFT 666
int oldMoveCase = -1;
int moveCase = GO;
//===================================//
//===================================//
//===================================//
//===================================//

//////////주행모드관련함수//////////
void lightDependsOnControlMode();
void sendMessageToRP_Shot();
void sendMessageToRP_Shot2();
void sendMessageToRP_Shot3();
void sendMessageToRP_Shot4();
void shotAndBack(int timeIntervalToShot);
void shotAndGO(int timeIntervalToShot);
void SHUTDOWN() ;
////////// 주행관련 함수//////////////
void lineTrace();
void lineBackTrace();
void testGoStraight();
void testBackStraight();
void motorMove();
void motorBackMove();
void brake(int n);
////////// 스텝모터관련 함수//////////
void setMoveCase(long timeIntervalToLineSensing);
void motorSetInit();
void stepper1_init(long motorspeed);
void stepper2_init(long motorspeed);
void stepper3_init(long motorspeed) ;
void motor1_move(long stepmove);
void motor2_move(long stepmove);
void motor3_move(long stepmove);
////////// 라인관련 함수////////////////
void readLine();
int lineSearch() ;
int backLineSearch();
////////// 카메라관련 함수//////////////
void camera_up(long stepmove) ;
void camera_down(long stepmove);
void cameraSetting();
/////////////////NFC&RFID///////////////
void nfc_init ();
unsigned char nfc_detect(void);
void setTheSeoga () ;
int findThisSeoga(int scanNFC);
bool isNewNFC (int scanNFC);
void whatShouldIDo(int scanNFC);
void readRFIDWithtime(int timeToCheckRfid);
////////////////////기타///////////////////
void light_shining();
void buttoncheck();

//===================================//
//===================================//
//=============MAIN FUNCTION==========//
//===================================//
//===================================//
void setup() {
  time_start = time_passed = time_start2 = time_passed2 = 0;
  controlMode = NewCaseStart  = 0;
  pinMode(SensorL, INPUT); pinMode(SensorC, INPUT);  pinMode(SensorR, INPUT);    // 적외선센서 Left, Center, Right
  pinMode(SensorBL, INPUT); pinMode(SensorBC, INPUT);  pinMode(SensorBR, INPUT);    // 적외선센서 Left, Center, Right
  pinMode(light1, OUTPUT); pinMode(light2, OUTPUT);  pinMode(light3, OUTPUT); //3번전구//전구
  pinMode(butttton, INPUT_PULLUP); //버튼
  Serial.begin(9600);
  digitalWrite(en_pin3, LOW); //EN 핀에 LOW신호를 모터를 사용시작합니다.
  nfc_init();
  stepper1_init(3000);   //스텝모터 초기화
  stepper2_init(3000);
  stepper3_init(5000);   //스텝모터 초기화
  setTheSeoga();
}
void loop() {
  readRFIDWithtime(600); //0.4초마다 한번씩 RFID를 센싱한다.
  lightDependsOnControlMode();  //모드의 상황을 불빛을 통해 보여준다. MODE_GO=1,MODE_SHOT_GO =2,MODE_SHOT_BACK=3,MODE_STOP=0
  // 주행버전
  switch (controlMode) {
    case MODE_GO:  //그냥 주행
      cameraSetting();
      testGoStraight();
      //lineTrace();
      break;
    case MODE_SHOT_GO:  //정방향 스캔 + 주행
      cameraSetting();
      shotAndGo(8000);
      break;
    case MODE_SHOT_BACK:   //역방향 스캔 + 주행
      cameraSetting();
      shotAndBack(8000);
      break;
    case MODE_STOP:  //종료
      SHUTDOWN();
  }
}

//===================================//
//===================================//
//==============FUNCTIONS=============//
//===================================//
//===================================//


//////////주행모드관련함수//////////
void lightDependsOnControlMode() {
  int k;
  switch (controlMode) {
    case MODE_GO:
      for (k = 0; k < 3; k++) digitalWrite(seg[k], num[0][k]);
      break;
    case MODE_SHOT_GO:
      for (k = 0; k < 3; k++) digitalWrite(seg[k], num[1][k]);
      break;
    case MODE_SHOT_BACK:
      for (k = 0; k < 3; k++) digitalWrite(seg[k], num[2][k]);
      break;
    case MODE_STOP:
      for (k = 0; k < 3; k++) digitalWrite(seg[k], num[3][k]);
      break;
  }
}
void sendMessageToRP_Shot() {
  switch (controlMode) {
    case MODE_SHOT_GO:
      //Serial.println("=======================================");
      //Serial.println("=======================================");
      //Serial.println(1);
      Serial.write(1);
      //Serial.println("=======================================");
      //Serial.println("=======================================");
      break;
    case MODE_SHOT_BACK:
      // Serial.println("=======================================");
      //Serial.println("=======================================");
      //Serial.println(2);
      Serial.write(2);
      // Serial.println("=======================================");
      // Serial.println("=======================================");
      break;
    default:
      break;
  }
}
void sendMessageToRP_Shot2() {
  switch (controlMode) {
    case MODE_SHOT_GO:
      //Serial.println("=======================================");
      //Serial.println("=======================================");
      //Serial.println(4);
      Serial.write(4);
      //Serial.println("=======================================");
      //Serial.println("=======================================");
      break;
    case MODE_SHOT_BACK:
      //Serial.println("=======================================");
      //Serial.println("=======================================");
      //Serial.println(3);
      Serial.write(3);
      //Serial.println("=======================================");
      //Serial.println("=======================================");
      break;
    default:
      break;
  }
}
void sendMessageToRP_Shot3() {
  switch (oldControlMode) {
    case MODE_SHOT_GO:
      //Serial.println("=======================================");
      //Serial.println("=======================================");
      //Serial.println(3);
      Serial.write(3);
      //Serial.println("=======================================");
      //Serial.println("=======================================");
      break;
    case MODE_SHOT_BACK:
      //Serial.println("=======================================");
      //Serial.println("=======================================");
      //Serial.println(4);
      Serial.write(4);
      //Serial.println("=======================================");
      //Serial.println("=======================================");
      break;
    default:
      break;
  }
}
void sendMessageToRP_Shot4() {
  //Serial.println("=======================================");
  //Serial.println("=======================================");
  //Serial.println(5);
  Serial.write(5);
  //Serial.println("=======================================");
  //Serial.println("=======================================");
}
void readmessagefromRP() {
}
void SHUTDOWN() {
  brake(200);
  light_shining();
}
void shotAndGo(int timeIntervalToShot) {
  if (!time_passed || time_passed > timeIntervalToShot) {
    time_start = millis();
    delay(1000);
    sendMessageToRP_Shot(); delay(1000);
    time_passed = millis() - time_start;
  }
  else {
    time_passed = millis() - time_start;
    //lineTrace();
    testGoStraight();
  }
}
void shotAndBack(int timeIntervalToShot) {
  if (!time_passed || time_passed > timeIntervalToShot) {
    time_start = millis();
    delay(1000);
    sendMessageToRP_Shot(); delay(1000);
    time_passed = millis() - time_start;
  }
  else {
    time_passed = millis() - time_start;
    testBackStraight();
    //backlinetrace();
  }
}
////////// 주행관련 함수//////////////
void lineTrace() {
  setMoveCase(long(200));
  motorSetInit();
  motorMove();
}
void lineBackTrace() {
  setBackMoveCase();
  motorSetInit();
  motorBackMove();
}
void testGoStraight() { ///서가 읽는 것을 테스트 하기위해 직진
  motor1_move(10000);
  motor2_move(10000);
  //Serial.println("자율주행중");
}
void testBackStraight() { ///서가 읽는 것을 테스트 하기위해 후진
  motor1_move(-10000);
  motor2_move(-10000);
  //Serial.println("빽빽자율주행중");
}
void motorMove() {
  motor1_move(10000);
  motor2_move(10000);
}
void motorMove1() {
  motor1_move(10000);
  motor2_move(10000);
}
void motorBackMove() {
  motor1_move(-10000);
  motor2_move(-10000);
}
void brake(int n) {
  delay(n);
  stepper1_init(0);
  stepper2_init(0);
  motor1_move(-1);
  motor2_move(-1);
}
////////// 스텝모터관련 함수//////////
void setMoveCase(long timeIntervalToLineSensing) { //moveCase를 setting하는 함수 - case가 바뀐 경우에만 motorInit을 해주기 위해서
  if (!time_passed3 || time_passed3 > timeIntervalToLineSensing) {
    time_start3 = millis();
    time_passed3 = millis() - time_start3;
    readLine();
    if (!l && !c && !r) {
      switch (oldMoveCase) {
        case GO:
          moveCase = GO;
          break;
        case TURN_RIGHT:
          moveCase = TURN_LEFT;
          break;
        case TURN_LEFT:
          moveCase = TURN_RIGHT;
          break;
      }
    }
    else {
      oldMoveCase = moveCase;
      if ((!l && c && !r) || (l && !c && r) || (l && c && r)) {
        moveCase = GO;
      }
      else if ((!l && !c && r) || (!l && c && r)) {
        moveCase = TURN_RIGHT;
      }
      else if ((l && !c && !r) || (l && c && !r)) {
        moveCase = TURN_LEFT;  //수정가능
      }
    }
  }
  else {
    time_passed3 = millis() - time_start3;
  }
}
void setBackMoveCase() { //moveCase를 setting하는 함수 - case가 바뀐 경우에만 motorInit을 해주기 위해서 //lineBack trace
  readLine();
  oldMoveCase = moveCase;
  if ((!bl && bc && !br)) moveCase = GO;
  else if ((!bl && !bc && br) || (!bl && bc && br)) moveCase = BACK_TURN_LEFT;
  else if ((bl && !bc && !br) || (bl && bc && !br)) moveCase = BACK_TURN_RIGHT;   //수정가능
}
void motorSetInit() { //case가 바뀐 경우에만 motorInit을 한다.
  if (oldMoveCase != moveCase) {
    switch (moveCase) {
      case GO:
        stepper1_init(3000);
        stepper2_init(3000);
        break;
      case TURN_RIGHT:
        stepper1_init(2000);
        stepper2_init(1300);
        break;
      case TURN_LEFT:
        stepper1_init(1300);
        stepper2_init(2000);
        break;
      case BACK:
        stepper1_init(3000);
        stepper2_init(3000);
        break;
      case BACK_TURN_RIGHT:
        stepper1_init(1000);
        stepper2_init(1500);
        break;
      case BACK_TURN_LEFT:
        stepper1_init(1500);
        stepper2_init(1000);
        break;
    }
  }
}
void stepper1_init(long motorspeed) {
  stepper1.setCurrentPosition(0);
  stepper1.setMaxSpeed(motorspeed);
  stepper1.setAcceleration(1500);
  stepper1.moveTo(100000);
}
void stepper2_init(long motorspeed) {
  stepper2.setCurrentPosition(0);
  stepper2.setMaxSpeed(motorspeed);
  stepper2.setAcceleration(1500);
  stepper2.moveTo(100000);
}
void stepper3_init(long motorspeed) {
  stepper3.setCurrentPosition(0);
  stepper3.setMaxSpeed(motorspeed);
  stepper3.setAcceleration(500);
  stepper3.moveTo(10000);
}
void motor1_move(long stepmove) {
  motorPosition1 = stepper1.currentPosition();
  stepper1.moveTo(motorPosition1 + stepmove);
  stepper1.run();
}
void motor2_move(long stepmove) {
  motorPosition2 = stepper2.currentPosition();
  stepper2.moveTo(motorPosition2 + stepmove);
  stepper2.run();
}
void motor3_move(long stepmove) {
  motorPosition3 = stepper3.currentPosition();
  stepper3.moveTo(motorPosition3 + stepmove);
  stepper3.run();
}

////////// 라인관련 함수////////////////
void readLine() {
  r = digitalRead(SensorR);
  l = digitalRead(SensorL);
  c = digitalRead(SensorC);
  br = digitalRead(SensorBR);
  bl = digitalRead(SensorBL);
  bc = digitalRead(SensorBC);
  digitalWrite(light11, l);
  digitalWrite(light22, c);
  digitalWrite(light33, r);
  /*Serial.print("  right : ");
    Serial.print(r);
    Serial.print("  || center : ");
    Serial.print(c);
    Serial.print("  || left : ");
    Serial.println(l);
    Serial.print(" Bright : ");
    Serial.print(br);
    Serial.print("  || Bcenter : ");
    Serial.print(bc);
    Serial.print("  || Bleft : ");
    Serial.println(bl);
    Serial.println("=================");*/
}
int lineSearch() {
}
int backLineSearch() {
}

////////// 카메라관련 함수//////////////
void camera_up(long stepmove) {
  motorPosition3 = stepper3.currentPosition();
  stepper3.moveTo(motorPosition3 + stepmove);
  stepper3.runToPosition();
  stepper3.stop();
  //for (int i = 0; i < 10; i++) {
  //Serial.println("camera up");
  //delay(500);
  //}
}
void camera_down(long stepmove) {
  motorPosition3 = stepper3.currentPosition();
  stepper3.moveTo(motorPosition3 - stepmove);
  stepper3.runToPosition();
  stepper3.stop();
  //for (int i = 0; i < 10; i++) {
  //Serial.println("camera down");
  //delay(500);
  //}*/
}
void cameraSetting() { //주행모드 변화종류는 5가지[[[[1>2,2>1(움직일 필요 없음),2>3(1칸내려감),3>1(움직일 필요 없음),3>2(1칸 내려감)]]]] 따라서 카메라 위치 변경필요
  if (NewCaseStart) { //케이스가 바뀐경우만 함수 시작 ex>제일 처음 서가를 만나면 많이 올라가고, 한 칸을 다 읽으면 한칸만 내려간다
    NewCaseStart = 0; //toggle변수 꺼줌
    //Serial.println("NewCaseStartNewCaseStartNewCaseStartNewCaseStart");
    if (oldControlMode == MODE_GO && controlMode == MODE_SHOT_GO) {
      if (beforeSeogaLevel == -1) {
        stepper3_init(10000);
        for (int i = 0; i < SEOGA[currentSeoga].level - 1; i++) { //서가의 칸 개수만큼 올라감
          camera_up(taketimefor1kan);
          delay(1000);
          //Serial.println("camera up");
        }
        beforeSeogaLevel = 0;
        ///Serial.print("칸 개수 : 서가 총칸");
        //Serial.println(SEOGA[currentSeoga].level);
      }
      else {
        stepper3_init(10000);
        for (int i = 0; i < beforeSeogaLevel - 1; i++) {
          camera_up(taketimefor1kan);
          delay(1000);
          //Serial.println("camera up");
        }
        ///Serial.print("칸 개수 : 비포서가레벨");
        //Serial.println(beforeSeogaLevel);
      }
    }
    else if ((oldControlMode == MODE_SHOT_GO && controlMode == MODE_GO) || (oldControlMode == MODE_SHOT_BACK  && controlMode == MODE_GO ) ) {
      delay(500);
      light_shining(); //한개의 서가가 끝난것을 알려주기 위해 불빛 3번 비춤
      sendMessageToRP_Shot3();
      delay(500);
    }
    else if ((oldControlMode == MODE_SHOT_GO  && controlMode == MODE_SHOT_BACK ) || (oldControlMode == MODE_SHOT_BACK  && controlMode == MODE_SHOT_GO)) {
      stepper3_init(10000);
      delay(1000);
      camera_down(long(taketimefor1kan / devideCamera)); //서가의 한 칸만큼 내려감
      delay(1000);
      sendMessageToRP_Shot2();
      delay(500);
      camera_down(long(taketimefor1kan / devideCamera));
    }
    motorInitial();
  }
}
void motorInitial() {
  switch (controlMode) {
    case MODE_GO:  //그냥 주행
      //Serial.println("go");
      stepper1_init(3000);
      stepper2_init(3000);
      break;
    case MODE_SHOT_GO:  //정방향 스캔 + 주행
      //Serial.println("------SHOT and GO------");
      stepper1_init(3000);
      stepper2_init(3000);
      break;
    case MODE_SHOT_BACK:   //역방향 스캔 + 주행
      //Serial.println("++++++SHOT and BACK");
      stepper1_init(3000);
      stepper2_init(3000);
      break;
    case MODE_STOP:  //종료
      //Serial.println("////STOP////");
      break;
  }
}
/////////////////NFC&RFID///////////////
void nfc_init () {
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    //Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  nfc.SAMConfig();
  // Serial.println("Waiting for an NFC Card ...");
}
unsigned char nfc_detect(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 20);
  //Serial.print("nfc_detect \n");
  if (success) {
    //Serial.print(" readPassiveTargetID  : ");
    //Serial.println(uid[1]) ;
    return uid[1];
  }
  return 0;
}
void setTheSeoga () {
  SEOGA[0] = {false, 3, 2, 162, 166};
  SEOGA[1] = {false, 3, 3, 132, 137};
}

int findThisSeoga(int scanNFC) {
  for (int k = 0; k < TOTALSEOGA; k++) {
    if (SEOGA[k].startCard == scanNFC || SEOGA[k].endCard == scanNFC) return k;
  }
  return -1;
}

bool isNewNFC (int scanNFC) {
  if ( scanNFC != 0 && currentNFC != scanNFC )
    return true;
  else
    return false;
}

void whatShouldIDo(int scanNFC) {
  if (scanNFC == AdminCard) {
    currentNFC = scanNFC;
    countOfDoneLevel = -1;
    controlMode = MODE_GO;
    stepper1_init(3000);
    stepper2_init(3000);
  }
  if (scanNFC ==   CameraCard3) {
    camera_up(long(10000));
  }
  if (scanNFC ==   CameraCard4) {
    camera_down(long(10000));
  }
  if (scanNFC ==   CameraCard5) {
    camera_up(long(30000));
  }
  if (scanNFC ==   CameraCard6) {
    camera_down(long(30000));
  }
  if ( scanNFC == FinalCard ) {
    currentNFC = scanNFC;
    countOfDoneLevel = -1;
    controlMode = MODE_STOP;
    sendMessageToRP_Shot4();
  }

  if ( isNewNFC(scanNFC) ) {   // 새로운 NFC카드가 스캔되면
    currentNFC = scanNFC;
    //Serial.println(currentNFC);
    NewCaseStart = 1;
    oldControlMode = controlMode;
    sendMessageToRP_Shot();
    int thisSeoga = findThisSeoga(scanNFC);     // 새로운 NFC의 소속 서가
    if (currentSeoga == thisSeoga) {            // 기존과 같은 서가라면
      if (countOfDoneLevel < SEOGA[thisSeoga].filledLevel ) {   // 해당 서가의 level을 모두 스캔하지 않았으면 방향 바꿔서 계속 스캔
        directionOfCar = !directionOfCar;                 // 방향 변경
        if (directionOfCar) {
          controlMode = MODE_SHOT_GO;          // 정주행할거야
          countOfDoneLevel++;
        } else {
          controlMode = MODE_SHOT_BACK;        // 역주행할거야
          countOfDoneLevel++;
        }
      } else {    // 해당 서가의 level을 모두 스캔했으면 정주행 고!
        SEOGA[thisSeoga].done = true;
        directionOfCar = 1;
        controlMode = MODE_GO;
      }
    } else if (scanNFC != 0 && thisSeoga != -1) {              // 기존과 다른 서가라면
      if (!SEOGA[thisSeoga].done) {   // 완료된 서가가 아니면 새로 시작!
        countOfDoneLevel = 0;
        if (beforeSeogaLevel != -1) {
          beforeSeogaLevel = SEOGA[currentSeoga].filledLevel;
        }
        currentSeoga = thisSeoga ;
        controlMode = MODE_SHOT_GO;          // 정주행할거야
        countOfDoneLevel++;
      } else {                        // 이미 완료된 서가라면 그냥 고!
        directionOfCar = 1;
        controlMode = MODE_GO;
      }
    }
  }
}
void readRFIDWithtime(int timeToCheckRfid) {
  if (!time_passed2 || time_passed2 > timeToCheckRfid) {
    time_start2 = millis();
    int scanNFC = nfc_detect() ;       // 새로 스캔한 NFC카드 번호
    whatShouldIDo(scanNFC);
    time_passed2 = millis() - time_start2;
  }
  else {
    time_passed2 = millis() - time_start2;
  }
}
////////////////////기타///////////////////
void light_shining() {
  int k, j;
  for (j = 0; j < 3; j++) {
    for (k = 0; k < 3; k++) digitalWrite(seg[k], num[3][k]); delay(300);
    for (k = 0; k < 3; k++) digitalWrite(seg[k], num[4][k]); delay(150);
  }
}
void buttoncheck() {
  button = digitalRead(butttton);
  if (button == 0 && old_button == 1) {
    if (controlMode == MODE_STOP) controlMode = MODE_GO;
    else if (controlMode == MODE_GO) controlMode = MODE_STOP;
  }
  old_button = button;
}
