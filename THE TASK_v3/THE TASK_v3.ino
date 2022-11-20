// Necessary Libraries
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
#include <String.h>
#include <HTTPClient.h>
#include <time.h>

// Constant Variables calculate date and time to track transactions in real time.
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

// Replace with your network credentials
const char* ssid = "DESKTOP-GUS6VE4";
const char* password = "pcgandhi";

// Initialize Telegram BOTS
// Unique BOT Tokens to control telegram bots
#define BOTtoken "5873588509:AAHc-8mO4dd76gfDPgSs4xBjQdxiizMysDg"  
//OTP bot to authenticate the user
#define OTPbot "5777026284:AAEtG3e16R9MefkFKhxENiO5x6kGKo3Z1PU"

// Unique chat ID to make sure bot interacts with a particular user
#define CHAT_ID "1023141522"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
UniversalTelegramBot otpbot(OTPbot, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//Global Variables
int default_user_num=0;
int user_num=0;
int otp = 0;


String GOOGLE_SCRIPT_ID = "AKfycbzBS9tMBeoboeMeKQ5AvVG6AaCjV6C7Gnyz6KTT67sNTMcgEjt6kLQCkQDdLLmyju5J";


// Handle what happens when we receive new messages. All commands to interact with the ATM Machine Bot are present here.
int handleNewMessages(int numNewMessages, String prev_text, int user_num) {

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    // Stores the commmands or previous message in these variables
    String c_cmd = bot.messages[i].text;
    String p_cmd = prev_text;

    // If the user types start command, then all the available options of ATM Machine are displayed
    if (c_cmd == "/start") {
      String welcome = "Welcome, User.\n";
      welcome += "Use the following commands to proceed.\n\n";
      welcome += "1. Enter Username \n";
      welcome += "2. New User \n";
      welcome += "3. Exit \n";
      String keyboard = "[[\"/enter_username\"],[\"/new_user\"], [\"/exit\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, welcome, "", keyboard);
    }
    // Each user has a unique username associated with him and stored in the spreadsheet
    // Option to enter the unique username of the user.
    else if (c_cmd == "/enter_username") {
      bot.sendMessage(chat_id, "Please enter your username.", "");
    }
    // Option available for new users to Register.
    else if (c_cmd == "/new_user") {
      bot.sendMessage(chat_id, "Enter your new Username.", "");
    }
    // Users can enter password to login.
    else if (c_cmd == "/enter_password"){
      bot.sendMessage(chat_id, "Please enter your password.", "");
      return user_num;
    }
    // Users can enter a new password incase they forget their current password
    else if (c_cmd == "/enter_new_password"){
      bot.sendMessage(chat_id, "Now enter your new password:", "");
      return user_num;
    }
    // Debit option where the user is asked the amount to be debited
    else if (c_cmd == "/debit"){
      String dmssg = "Enter Amount to be debited (in Multiples of Rs 100) \n";
      dmssg += "(Example :Press 2 for Rs 200)\n";
      bot.sendMessage(chat_id, dmssg, "");
      return user_num;
    }
    // Credit option where the user is asked the amount to be credited in his account
    else if (c_cmd == "/credit"){
      String cmssg = "Enter Amount to be credited (in Multiples of Rs 100) \n";
      cmssg += "(Example :Press 2 for Rs 200)\n";
      bot.sendMessage(chat_id, cmssg, "");
      return user_num;
    }

    // code for generating a random otp through the OTP Bot and authenthicating the user.
    else if(c_cmd == "/forgot_password"){

      otp = 1000+(rand()%9000); 
      String otpcheck="Please Enter the OTP sent to you: \n";
      bot.sendMessage(chat_id, otpcheck, "");

      String mssgwithotp = "Your OTP : " + String(otp) + "\n";
      mssgwithotp += "Use this OTP to change your password. \n";
      otpbot.sendMessage(chat_id, mssgwithotp, "");
      
      return user_num;
      }

    // Verifies whether the username enterd by user is present in the database or not, since each username is unique
    else if (p_cmd == "/enter_username") {
      String username=c_cmd;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        
        // To accesss the spreadsheet database of users
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?data="+String(username);
        Serial.println("Making a request to spreadheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        // row value variable stores the row number, where the user data is saved in the spreadsheet, as it is required at multiple tasks.
        String row_val;

        if (user > 0) {
        row_val = http.getString();
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //If username is found, user is asked to enter his password or an additional option to change his password, in case if he forgot is also available.
      int row = row_val.toInt();
      if(row!=0){
        String usercheck="Username Found.\n";
        usercheck+="Welcome " + username +" .\n\n";
        usercheck+="Please Proceed:\n\n";
        usercheck += "1. Enter Password \n";
        usercheck += "2. Forgot Password \n";
        usercheck += "3. Exit \n";
        String keyboard = "[[\"/enter_password\"],[\"/forgot_password\"], [\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, usercheck, "",keyboard);
      }
      // If the username is not found in the database, the ATM machine stops and asks user to try again with correct username.
      else{
        String usercheck="Invalid User \n";
        usercheck+="Username not found in the database. \n";
        usercheck+="Please Start Again:\n\n";
        usercheck += "1. Start \n";
        usercheck += "2. Exit \n";
        String keyboard = "[[\"/start\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, usercheck, "",keyboard);
      }
      return row;

    }}

    // If a new user uses the ATM machine, the user is asked to enter a new unique username which should not be already present in the database
    else if (p_cmd == "/new_user") {
      String newusername=c_cmd;
      int row=1;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        // To accesss the spreadsheet database of users
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?data="+String(newusername);
        Serial.println("Making a request to spreadheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String row_val;
        if (user > 0) {
        row_val = http.getString();
      }
      // if database spreadsheet cannot be accessed
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //if the username is unique it is saved in the spreadseet and the user is asked to enter a password
      row = row_val.toInt();
      if(row==0){
        String newuser="This username is unique.\n";
        newuser+="Saving this username...\n";
        newuser+="Please Wait...\n";
        bot.sendMessage(chat_id, newuser, "");
        }
        //if the username already exists in the spreadsheet, the user is asked to try again.
      else{
        String newuser="Username already exists.\n";
        newuser+="Please Start Again and select a different username:\n\n";
        newuser+="1. Start \n";
        newuser+="2. Exit \n";
        String keyboard = "[[\"/start\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, newuser, "",keyboard);
        }
    }
    // code to save the unique username to spreadsheet
    if (WiFi.status() == WL_CONNECTED) {
      if(row==0){
        HTTPClient http;
        // To accesss the spreadsheet database of users
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?newuser="+String(newusername);
        Serial.println("Making a request to spreadheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String row_val;
        if (user > 0) { 
        row_val = http.getString();
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      int new_row = row_val.toInt();
        String newuser="Username Saved Successfully. \n";
        newuser+="1. Enter New Password.\n\n";
        newuser+="2. Exit";
        String keyboard = "[[\"/enter_new_password\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, newuser, "",keyboard);
      return new_row;
      }
    }}


    //code to verify the password entered by the user.
    else if (p_cmd == "/enter_password") {
      String userpass=c_cmd;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        // To accesss the spreadsheet database of users
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?passrow="+String(user_num);
        Serial.println("Making a request to spreadsheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        
        String pass_sheet; // contains the password present in the spreadsheet
        
        if (user > 0) { //Check for the returning code
        pass_sheet = http.getString();
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //Compare the password entered by user and password present at the spreadsheet. If same Login Successful
      // User is asked to debit, credit or check his balance
      if(pass_sheet==userpass){
        String passcheck="Correct Password.\n";
        passcheck+="Select Required Action \n\n";
        passcheck+="1. Debit \n";
        passcheck+="2. Credit \n";
        passcheck+="3. Check Balance \n";
        passcheck+="4. Check Transaction History\n";
        passcheck+="5. Exit \n";
        String keyboard = "[[\"/debit\"],[\"/credit\"],[\"/check_balance\"],[\"/transaction_history\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, passcheck, "",keyboard);
        return user_num;
        }
      // if incorrect password in entered, the user is asked to try again.
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
    //code to save the new password as entered by new users at time of registration, same code is also used at the time of forgot password command to update existing user password
    else if (p_cmd == "/enter_new_password") {
      String newuserpass=c_cmd;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?newpassrow="+String(user_num)+"&newpass="+String(newuserpass);
        // To accesss the spreadsheet database of users
        Serial.println("Making a request to spreadheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String pass_sheet;
        if (user > 0) { 
        
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      String passtext = "Password Saved Successfully.\n";
      passtext+= "You can login using your new username and password\n\n";
      passtext+= "1. Start \n";
      passtext+= "2. Exit \n";
      String keyboard = "[[\"/start\"],[\"/exit\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, passtext, "",keyboard);
    }}
    //Code to verify OTP as sent by the OTPbot and otp enterd by the user. If they match, the user can reset his password
    else if (p_cmd == "/forgot_password"){
      String otpstr = c_cmd;
      int userotp = otpstr.toInt();
      if(userotp==otp){
      String cotp = "OTP verified successfully.\n";
      cotp+= "Proceed to change your password\n\n";
      cotp+= "1. Enter New Password \n";
      cotp+= "2. Abort and Exit \n";
      String keyboard = "[[\"/enter_new_password\"],[\"/exit\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, cotp, "",keyboard);
      }
      //if OTP do not match, user is asked Start Again.
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


    // Here the new balance of the user is calculated after the amount entered is debited from the user's account.
    else if (p_cmd == "/debit") {
      String d_amount=c_cmd;
      int new_bal, d_amt;
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      //break;
      }
      char timeStringBuff[50]; //50 chars should be enough
      strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
      String asString(timeStringBuff);
      asString.replace(" ", "-");
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        // To accesss the spreadsheet database of users
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?user_row="+String(user_num);
        Serial.println("Making a request to spreadheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String bal_val;
        if (user > 0) {
        bal_val = http.getString();
        }      
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      //if debit is successful, the Transaction Summary is displayed on the Serial Monitor as well as through the ATM Machine Bot, and also updated on the spreadsheet
      int bal = bal_val.toInt();
      d_amt = d_amount.toInt();
      d_amt = 100*d_amt;
      new_bal = bal-d_amt;
      if(new_bal>=0){
        String dtext="Transaction Successful.\n";
        dtext+="Transaction Details: \n";
        dtext+="Transaction Time : " +String(asString)+" \n";
        dtext+="Initial Balance : Rs "+String(bal) +" \n";
        dtext+="Debitted Amount : Rs "+String(d_amt) +" \n";
        dtext+="Remaining Balance : Rs "+String(new_bal) +" \n";
        Serial.println();
        Serial.println(dtext);
        bot.sendMessage(chat_id, dtext, "");

      }
      // The debit transaction fails if the user tries to debit more amount than available in his account.
      else{
        String dtext="Transaction Failed.\n";
        dtext+="Your Balance : Rs "+ String(bal)  +" \n";
        dtext+="You can debit money less than your balance.\n\n";
        dtext+="1. Start \n";
        dtext+="2. Exit \n";
        Serial.println();
        Serial.println(dtext);
        String keyboard = "[[\"/start\"], [\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, dtext, "",keyboard);
      }

    }
    //Code to update spreadsheet
    if (WiFi.status() == WL_CONNECTED) {

      if(new_bal>=0){
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?user_row_val="+String(user_num)+"&act=debit"+"&amount="+String(d_amt)+"&new_bal="+String(new_bal)+"&ttime="+String(asString);
        // To accesss the spreadsheet database of users
        Serial.println("Making a request to spreadheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        if (user > 0) { 
        String textd = "Select Required Action:\n\n";
        textd+="1. Debit \n";
        textd+="2. Credit \n";
        textd+="3. Check Balance \n";
        textd+="4. Check Transaction History \n";
        textd+="5. Exit \n";
        String keyboard = "[[\"/debit\"],[\"/credit\"],[\"/check_balance\"],[\"/transaction_history\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, textd, "",keyboard);

      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
    }}
    return user_num;
    
    }

    // Here the new balance of the user is calculated after the amount entered is credited from th user's account.
    else if (p_cmd == "/credit") {
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      }
      char timeStringBuff[50]; //50 chars should be enough
      strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
      String asString(timeStringBuff);
      asString.replace(" ", "-");
      String c_amount=c_cmd;
      int new_bal, c_amt;
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?user_row="+String(user_num);
        // To accesss the spreadsheet database of users
        Serial.println("Making a request to spreadheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String bal_val;

        if (user > 0) {
        bal_val = http.getString();
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
    //displays the transaction summary in serial monitor as well as through the ATM Machine Bot if the credit transaction is successful.
      int bal = bal_val.toInt();
      c_amt = c_amount.toInt();
      c_amt = c_amt*100;
      new_bal = bal+c_amt;
      if(new_bal>=0){
        String ctext="Transaction Successful.\n";
        ctext+="Transaction Details: \n";
        ctext+="Transaction Time : " +String(asString)+" \n";
        ctext+="Initial Balance : Rs "+String(bal) +" \n";
        ctext+="Credited Amount : Rs "+String(c_amt) +" \n";
        ctext+="Remaining Balance : Rs "+String(new_bal) +" \n";
        Serial.println();
        Serial.println(ctext);
        bot.sendMessage(chat_id, ctext, "");
      }
      //The user is asked to try again, if invalid amount is credited.
      else{
        String ctext="Transaction Failed.\n";
        ctext+="Your Balance : Rs "+ String(bal)  +" \n";
        ctext+="You cannot credit money in negative amount.\n";
        Serial.println();
        Serial.println(ctext);
        String keyboard = "[[\"/start\"], [\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, ctext, "",keyboard);
      }

    }
    // to update the transaction summary to the spreadsheet
    if (WiFi.status() == WL_CONNECTED) {

      if(new_bal>=0){
        HTTPClient http;
        // To accesss the spreadsheet database of users
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?user_row_val="+String(user_num)+"&act=credit"+"&amount="+String(c_amt)+"&new_bal="+String(new_bal)+"&ttime="+String(asString);
        Serial.println("Making a request to spreadheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();

        if (user > 0) { 
        String textc = "Select Required Action:\n\n";
        textc+="1. Debit \n";
        textc+="2. Credit \n";
        textc+="3. Check Balance \n";
        textc+="4. Check Transaction History \n";
        textc+="5. Exit \n";
        String keyboard = "[[\"/debit\"],[\"/credit\"],[\"/check_balance\"],[\"/transaction_history\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, textc, "",keyboard);
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();

    }}
    return user_num;
    }

    // to check the available balance present in the user's account
    else if (c_cmd == "/check_balance") {
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?user_row="+String(user_num);
        // To accesss the spreadsheet database of users
        Serial.println("Making a request to spreadheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String bal_val;

        if (user > 0) {
        bal_val = http.getString();
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //The balance is displayed on the serial monitor as well as through the ATM Machine Bot
      int bal = bal_val.toInt();
      
        String dtext="Here is your Account Balance : \n";
        dtext+=" Balance : Rs "+String(bal) +" \n";
        Serial.println();
        Serial.println(dtext);
        bot.sendMessage(chat_id, dtext, "");

        String textc = "Select Required Action:\n\n";
        textc+="1. Debit \n";
        textc+="2. Credit \n";
        textc+="3. Check Balance \n";
        textc+="4. Check Transaction History \n";
        textc+="5. Exit \n";
        String keyboard = "[[\"/debit\"],[\"/credit\"],[\"/check_balance\"],[\"/transaction_history\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, textc, "",keyboard);

        return user_num;

    }}

    else if (c_cmd == "/transaction_history") {
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?row_of_user="+String(user_num);
        // To accesss the spreadsheet database of users
        Serial.println("Making a request to spreadheet...");
        http.begin(url.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int user = http.GET();
        String trans_his;

        if (user > 0) {
        trans_his = http.getString();
      }
      else {
      Serial.println("Error on HTTP request");
      }
      http.end();
      
      //The Complete Transaction History is displayed on the serial monitor as well as through the ATM Machine Bot
      
        String ttext="Here is your Transaction History : \n";
        ttext+=String(trans_his);
        Serial.println();
        Serial.println(ttext);
        bot.sendMessage(chat_id, ttext, "");

        String textc = "Select Required Action:\n\n";
        textc+="1. Debit \n";
        textc+="2. Credit \n";
        textc+="3. Check Balance \n";
        textc+="4. Check Transaction History \n";
        textc+="5. Exit \n";
        String keyboard = "[[\"/debit\"],[\"/credit\"],[\"/check_balance\"],[\"/transaction_history\"],[\"/exit\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, textc, "",keyboard);

        return user_num;

    }}

    // To exit the ATM Machine BOT Interface
    else if (c_cmd == "/exit"){
      
      String keyboard = "[[\"/start\"]]";
      String exittext = "Successfully Exited \n";
      exittext += "Press Start to Start ATM Again. \n";
      bot.sendMessageWithReplyKeyboard(chat_id, exittext, "",keyboard);
      return user_num;
    }
    
  }
  return 0;
}

void setup() {
  Serial.begin(115200);

  
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
  // Configuration of date and time for saving transaction details.
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  // this variable saves the second last input entered to the ATM Machine Bot to identify the task choosen by the user.
  String prev_text = bot.messages[0].text;
  // Code to update the Telegram Bot Message after every 1 second to know if the user entered any new command
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      // if the ATM Machine bot got any command or response from the user, that command is sent to the below function for further processing of the ATM Machine.
      Serial.println("got response from Telegram ATM Bot");
      user_num = handleNewMessages(numNewMessages, prev_text, default_user_num);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      default_user_num=user_num;
    }
    lastTimeBotRan = millis();
  }
}
