/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
  Example based on the Universal Arduino Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino
*/

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
#include <String.h>
#include <HTTPClient.h>
#include <time.h>
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

// Replace with your network credentials
const char* ssid = "Y73";
const char* password = "pcgandhi";

// Initialize Telegram BOT
#define BOTtoken "5873588509:AAHc-8mO4dd76gfDPgSs4xBjQdxiizMysDg"  // your Bot Token (Get from Botfather)

#define OTPbot "5777026284:AAEtG3e16R9MefkFKhxENiO5x6kGKo3Z1PU"

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1023141522"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
UniversalTelegramBot otpbot(OTPbot, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

int default_user_num=0;
int user_num=0;
int otp = 0;


String GOOGLE_SCRIPT_ID = "AKfycbwH8ra8q4ixdpjifF2DUQjpivo3BAz7h3NCZL_49yAjlK3ujPQbrvPT82p4-GqjVnzz";    // change Gscript ID


// Handle what happens when you receive new messages
int handleNewMessages(int numNewMessages, String prev_text, int user_num) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    // Print the received message
    String c_cmd = bot.messages[i].text;
    String p_cmd = prev_text;

    //String from_name = bot.messages[i].from_name;

    if (c_cmd == "/start") {
      String welcome = "Welcome, User.\n";
      welcome += "Use the following commands to proceed.\n\n";
      welcome += "1. Enter Username \n";
      welcome += "2. New User \n";
      welcome += "3. Exit \n";
      String keyboard = "[[\"/enter_username\"],[\"/new_user\"], [\"/exit\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, welcome, "", keyboard);
    }

    else if (c_cmd == "/enter_username") {
      bot.sendMessage(chat_id, "Please enter your username.", "");
      //ledState = HIGH;
      //digitalWrite(ledPin, ledState);
    }

    else if (c_cmd == "/new_user") {
      bot.sendMessage(chat_id, "Enter your new Username.", "");
      //ledState = HIGH;
      //digitalWrite(ledPin, ledState);
    }

    else if (c_cmd == "/enter_password"){
      bot.sendMessage(chat_id, "Please enter your password.", "");
      //ledState = HIGH;
      //digitalWrite(ledPin, ledState);
      return user_num;
    }

    else if (c_cmd == "/enter_new_password"){
      bot.sendMessage(chat_id, "Now enter your new password:", "");
      //ledState = HIGH;
      //digitalWrite(ledPin, ledState);
      return user_num;
    }

    else if (c_cmd == "/debit"){
      String dmssg = "Enter Amount to be debited (in Multiples of Rs 100) \n";
      dmssg += "(Example :Press 2 for Rs 200)\n";
      bot.sendMessage(chat_id, dmssg, "");
      //ledState = HIGH;
      //digitalWrite(ledPin, ledState);
      return user_num;
    }

    else if (c_cmd == "/credit"){
      String cmssg = "Enter Amount to be credited (in Multiples of Rs 100) \n";
      cmssg += "(Example :Press 2 for Rs 200)\n";
      bot.sendMessage(chat_id, cmssg, "");
      //ledState = HIGH;
      //digitalWrite(ledPin, ledState);
      return user_num;
    }

    else if(c_cmd == "/forgot_password"){

      otp = 1000+(rand()%9000); 

      //int otpnewmssg = otpbot.getUpdates(otpbot.last_message_received + 1);
      String otpcheck="Please Enter the OTP sent to you: \n";
      bot.sendMessage(chat_id, otpcheck, "");


      String mssgwithotp = "Your OTP : " + String(otp) + "\n";
      mssgwithotp += "Use this OTP to change your password. \n";
      otpbot.sendMessage(chat_id, mssgwithotp, "");
      
      return user_num;
      }
      // Print the received message




    else if (p_cmd == "/enter_username") {
      String username=c_cmd;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?data="+String(username);
        //Serial
        Serial.println("Making a request");
        http.begin(url.c_str()); //Specify the URL and certificate
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String row_val;
        //String var="";
        if (user > 0) { //Check for the returning code
        row_val = http.getString();
        //Serial.println("user");
        //Serial.println(user);
        //Serial.println(row_val);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //Serial.println(row_val);
      int row = row_val.toInt();
      Serial.println(row);
      if(row!=0){
        String usercheck="Username Found.\n";
        usercheck+="Welcome " + username +" .\n\n";
        usercheck+="Please Proceed:\n\n";
        usercheck += "1. Enter Password \n";
        usercheck += "2. Forgot Password \n";
        usercheck += "3. Exit \n";
        String keyboard = "[[\"/enter_password\"],[\"/forgot_password\"], [\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, usercheck, "",keyboard);
        //bot.sendMessage(chat_id, "username found", "");
      }
      else{
        String usercheck="Username not found.\n";
        usercheck+="Please Start Again:\n\n";
        usercheck += "1. Start \n";
        usercheck += "2. Exit \n";
        String keyboard = "[[\"/start\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, usercheck, "",keyboard);
      }
      return row;

    }}


    else if (p_cmd == "/new_user") {
      String newusername=c_cmd;
      int row=1;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?data="+String(newusername);
        //Serial
        Serial.println("Making a request");
        http.begin(url.c_str()); //Specify the URL and certificate
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String row_val;
        //String var="";
        if (user > 0) { //Check for the returning code
        row_val = http.getString();
        Serial.println("user");
        Serial.println(user);
        Serial.println(row_val);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //Serial.println(row_val);
      row = row_val.toInt();
      Serial.println(row);
      if(row==0){
        String newuser="This username is unique.\n";
        newuser+="Saving this username...\n";
        newuser+="Please Wait...\n";
        //String keyboard = "[[\"/enter_password\"],[\"/forgot_password?\"]]";
        bot.sendMessage(chat_id, newuser, "");
        //bot.sendMessage(chat_id, "username found", "");
        }
      else{
        String newuser="Username already exists.\n";
        newuser+="Please Start Again and select a different username:\n\n";
        newuser+="1. Start \n";
        newuser+="2. Exit \n";
        //newuser+= "/start \n";
        String keyboard = "[[\"/start\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, newuser, "",keyboard);
        }
    }

    if (WiFi.status() == WL_CONNECTED) {
      if(row==0){
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?newuser="+String(newusername);
        //Serial
        Serial.println("Making a request");
        http.begin(url.c_str()); //Specify the URL and certificate
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String row_val;
        //String var="";
        if (user > 0) { //Check for the returning code
        row_val = http.getString();
        Serial.println("user");
        Serial.println(user);
        Serial.println(row_val);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      
      //Serial.println(row_val);
      int new_row = row_val.toInt();
      Serial.println(new_row);
        String newuser="Username Saved Successfully. \n";
        newuser+="1. Enter New Password.\n\n";
        newuser+="2. Exit";
        String keyboard = "[[\"/enter_new_password\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, newuser, "",keyboard);
        //bot.sendMessage(chat_id, "username found", "");
      return new_row;
      }
    }}



    else if (p_cmd == "/enter_password") {
      String userpass=c_cmd;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        //Serial.println("User_num");
        //Serial.println(user_num);
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?passrow="+String(user_num);
        //Serial
        Serial.println("Making a request");
        http.begin(url.c_str()); //Specify the URL and certificate
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String pass_sheet;
        //String var="";
        if (user > 0) { //Check for the returning code
        pass_sheet = http.getString();
        Serial.println("user");
        Serial.println(user);
        Serial.println(pass_sheet);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //Serial.println(row_val);
      if(pass_sheet==userpass){
        String passcheck="Correct Password.\n";
        passcheck+="Select Required Action \n\n";
        passcheck+="1. Debit \n";
        passcheck+="2. Credit \n";
        passcheck+="3. Check Balance \n";
        passcheck+="4. Exit \n";
        String keyboard = "[[\"/debit\"],[\"/credit\"],[\"/check_balance\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, passcheck, "",keyboard);
        //bot.sendMessage(chat_id, "username found", "");
        return user_num;
        }
      else{
        String passcheck="Password Incorrect.\n";
        passcheck+="Please Start Again.\n\n";
        passcheck+="1. Start \n";
        passcheck+="2. Exit \n";
        //passcheck+="/start\n";
        String keyboard = "[[\"/start\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, passcheck, "",keyboard);
        }

    }}

    else if (p_cmd == "/enter_new_password") {
      String newuserpass=c_cmd;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        Serial.println("User_num");
        Serial.println(user_num);
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?newpassrow="+String(user_num)+"&newpass="+String(newuserpass);
        //Serial
        Serial.println("Making a request");
        http.begin(url.c_str()); //Specify the URL and certificate
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String pass_sheet;
        //String var="";
        if (user > 0) { //Check for the returning code
        //pass_sheet = http.getString();
        Serial.println("user");
        Serial.println(user);
        //Serial.println(pass_sheet);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //Serial.println(row_val);
      String passtext = "Password Saved Successfully.\n";
      //passtext+= "Opening Balance is Rs 15000/-\n";
      passtext+= "You can login using your new username and password\n\n";
      passtext+= "1. Start \n";
      passtext+= "2. Exit \n";
      String keyboard = "[[\"/start\"],[\"/exit\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, passtext, "",keyboard);


    }}

    else if (p_cmd == "/forgot_password"){
      String otpstr = c_cmd;
      Serial.println("Inside pcmd");
      int userotp = otpstr.toInt();
      if(userotp==otp){
      String cotp = "OTP verified successfully.\n";
      cotp+= "Proceed to change your password\n\n";
      cotp+= "1. Enter New Password \n";
      cotp+= "2. Abort and Exit \n";
      String keyboard = "[[\"/enter_new_password\"],[\"/exit\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, cotp, "",keyboard);
      }
      else{
      String cotp = "Your OTP is incorrect.\n";
      cotp+= "Please Start Again.\n\n";
      cotp+= "1. Start \n";
      cotp+= "2. Exit \n";
      String keyboard = "[[\"/start\"],[\"/exit\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, cotp, "",keyboard);
      }
      return user_num;
    }

    else if (p_cmd == "/debit") {
      String d_amount=c_cmd;
      int new_bal, d_amt;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?user_row="+String(user_num);
        //Serial
        Serial.println("Making a request");
        http.begin(url.c_str()); //Specify the URL and certificate
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String bal_val;
        //String var="";
        if (user > 0) { //Check for the returning code
        bal_val = http.getString();
        
        //Serial.println("user");
        //Serial.println(user);
        //Serial.println(row_val);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //Serial.println(row_val);
      int bal = bal_val.toInt();
      d_amt = d_amount.toInt();
      d_amt = 100*d_amt;
      new_bal = bal-d_amt;
      Serial.println(bal);
      if(new_bal>=0){
        String dtext="Transaction Successful.\n";
        dtext+="Transaction Details: \n";
        dtext+="Initial Balance : Rs "+String(bal) +" \n";
        dtext+="Debitted Amount : Rs "+String(d_amt) +" \n";
        dtext+="Remaining Balance : Rs "+String(new_bal) +" \n";
        //String keyboard = "[[\"/enter_password\"],[\"/forgot_password\"]]";
        bot.sendMessage(chat_id, dtext, "");
        //bot.sendMessage(chat_id, "username found", "");
      }
      else{
        String dtext="Transaction Failed.\n";
        dtext+="Your Balance : Rs "+ String(bal)  +" \n";
        dtext+="You can debit money less than your balance.\n\n";
        dtext+="1. Start \n";
        dtext+="2. Exit \n";
        String keyboard = "[[\"/start\"], [\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, dtext, "",keyboard);
      }

    }

    if (WiFi.status() == WL_CONNECTED) {
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      //break;
      }
      char timeStringBuff[50]; //50 chars should be enough
      strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
      String asString(timeStringBuff);
      asString.replace(" ", "-");
    Serial.println("Time:");
    Serial.println(asString);
      if(new_bal>=0){
        Serial.println(user_num);
        Serial.println(d_amt);
        Serial.println(new_bal);
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?user_row_val="+String(user_num)+"&act=debit"+"&amount="+String(d_amt)+"&new_bal="+String(new_bal)+"&ttime="+String(asString);
        //Serial
        Serial.println("Making a request");
        http.begin(url.c_str()); //Specify the URL and certificate
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        //String bal_val;
        //String var="";
        if (user > 0) { //Check for the returning code
        //bal_val = http.getString();
        String textd = "Select Required Action:\n\n";
        textd+="1. Debit \n";
        textd+="2. Credit \n";
        textd+="3. Check Balance \n";
        textd+="4. Exit \n";
        String keyboard = "[[\"/debit\"],[\"/credit\"],[\"/check_balance\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, textd, "",keyboard);
        Serial.println("Works...");
        //Serial.println("user");
        //Serial.println(user);
        //Serial.println(row_val);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //Serial.println(row_val);

    }}
    return user_num;
    
    }

    else if (p_cmd == "/credit") {
      String c_amount=c_cmd;
      int new_bal, c_amt;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?user_row="+String(user_num);
        //Serial
        Serial.println("Making a request");
        http.begin(url.c_str()); //Specify the URL and certificate
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String bal_val;
        //String var="";
        if (user > 0) { //Check for the returning code
        bal_val = http.getString();
        
        //Serial.println("user");
        //Serial.println(user);
        //Serial.println(row_val);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //Serial.println(row_val);
      int bal = bal_val.toInt();
      c_amt = c_amount.toInt();
      c_amt = c_amt*100;
      new_bal = bal+c_amt;
      Serial.println(bal);
      if(new_bal>=0){
        String ctext="Transaction Successful.\n";
        ctext+="Transaction Details: \n";
        ctext+="Initial Balance : Rs "+String(bal) +" \n";
        ctext+="Credited Amount : Rs "+String(c_amt) +" \n";
        ctext+="Remaining Balance : Rs "+String(new_bal) +" \n";
        //String keyboard = "[[\"/enter_password\"],[\"/forgot_password\"]]";
        bot.sendMessage(chat_id, ctext, "");
        //bot.sendMessage(chat_id, "username found", "");
      }
      else{
        String ctext="Transaction Failed.\n";
        ctext+="Your Balance : Rs "+ String(bal)  +" \n";
        ctext+="You cannot credit money in negative amount.\n";
        String keyboard = "[[\"/start\"], [\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, ctext, "",keyboard);
      }

    }

    if (WiFi.status() == WL_CONNECTED) {
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      }
      char timeStringBuff[50]; //50 chars should be enough
      strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
      String asString(timeStringBuff);
      asString.replace(" ", "-");
      //Serial.print("Time:");
      //Serial.println(asString);
      if(new_bal>=0){
        Serial.println(user_num);
        Serial.println(c_amt);
        Serial.println(new_bal);
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?user_row_val="+String(user_num)+"&act=debit"+"&amount="+String(c_amt)+"&new_bal="+String(new_bal)+"&ttime="+String(asString);
        //Serial
        Serial.println("Making a request");
        http.begin(url.c_str()); //Specify the URL and certificate
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        //String bal_val;
        //String var="";
        if (user > 0) { //Check for the returning code
        //bal_val = http.getString();
        String textc = "Select Required Action:\n\n";
        textc+="1. Debit \n";
        textc+="2. Credit \n";
        textc+="3. Check Balance \n";
        textc+="4. Exit \n";
        String keyboard = "[[\"/debit\"],[\"/credit\"],[\"/check_balance\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, textc, "",keyboard);
        Serial.println("Works...");
        //Serial.println("user");
        //Serial.println(user);
        //Serial.println(row_val);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //Serial.println(row_val);

    }}
    
    }

    else if (c_cmd == "/check_balance") {
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?user_row="+String(user_num);
        //Serial
        Serial.println("Making a request");
        http.begin(url.c_str()); //Specify the URL and certificate
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String bal_val;
        //String var="";
        if (user > 0) { //Check for the returning code
        bal_val = http.getString();
        
        //Serial.println("user");
        //Serial.println(user);
        //Serial.println(row_val);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //Serial.println(row_val);
      int bal = bal_val.toInt();
      
        String dtext="Here is your Account Balance : \n";
        dtext+=" Balance : Rs "+String(bal) +" \n";
        //String keyboard = "[[\"/enter_password\"],[\"/forgot_password\"]]";
        bot.sendMessage(chat_id, dtext, "");
        //bot.sendMessage(chat_id, "username found", "");

        String textc = "Select Required Action:\n\n";
        textc+="1. Debit \n";
        textc+="2. Credit \n";
        textc+="3. Check Balance \n";
        textc+="4. Exit \n";
        String keyboard = "[[\"/debit\"],[\"/credit\"],[\"/check_balance\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, textc, "",keyboard);

        return user_num;

    }}
    else if (c_cmd == "/exit"){
      
      String keyboard = "[[\"/start\"]]";
      String exittext = "Successfully Exited \n";
      exittext += "Press Start to Start ATM Again. \n";
      bot.sendMessageWithReplyKeyboard(chat_id, exittext, "",keyboard);
      //ledState = HIGH;
      //digitalWrite(ledPin, ledState);
      return user_num;
    }
    
  }
  return 0;
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  String prev_text = bot.messages[0].text;
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      user_num = handleNewMessages(numNewMessages, prev_text, default_user_num);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      default_user_num=user_num;
    }
    lastTimeBotRan = millis();
  }
}
// void loop() {
  
//   String prev_text = bot.messages[0].text;
//   //Serial.println(millis());
//   //Serial.println("lasttimebot ran");
//   //Serial.println(lastTimeBotRan + botRequestDelay);
//   if (millis() > lastTimeBotRan + botRequestDelay)  {
//     //Serial.println("randi");
//     int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

//     while(numNewMessages) {
//       Serial.println("got response");
//       user_num = handleNewMessages(numNewMessages, prev_text, default_user_num);
//       numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//       default_user_num = user_num;
//     }
//     lastTimeBotRan = millis();
//   }
// }