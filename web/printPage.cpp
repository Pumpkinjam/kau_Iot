#include "secret.h"
// <meta http-equiv="refresh" content="3"><meta http-equiv="refresh" content="3"> 3초마다 새로고침
String login_html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset=="UTF-8">
    <title>Document</title>
    <meta name="viewport" content="width=device-width, height=device-height, minimum-scale=1.0, maximum-scale=1.0, initial-scale=1.0"> 
    <style>
        
    </style>
</head>
    <meta http-equiv="refresh" content="3"><meta http-equiv="refresh" content="3">       
    <body style="background-color: #ededed; padding: 0; margin: 0;"> 
        <div style="width: 780px; height: 550px; margin: 0 auto;" class="loginbox">
            <h2 style="width: 100%;">Login</h2>
            <form style="width: 100%; height: 450px; background-color: #fff; padding: 60px; box-sizing: border-box;" action="#">
              <fieldset style="border: none;">
                <legend style="position: absolute; left: -999em;">로그인</legend>
                <label style="display: block; font-size: 12px; color: #333; margin-bottom: 10px;" for="loginid">id</label>
                <input style="width: 600px; display: block; height: 50px; margin-bottom: 25px; border: none; background-color: #ededed; padding: 0 20px; box-sizing: border-box;" type="text" id="loginid" placeholder="id">
                <label style="display: block; font-size: 12px; color: #333; margin-bottom: 10px;" for="loginpw">password</label>
                <input style="width: 600px; display: block; height: 50px; margin-bottom: 25px; border: none; background-color: #ededed; padding: 0 20px; box-sizing: border-box;" type="password" id="loginpw" placeholder="password">
                <ul style="width: 100%; text-align: right; margin-bottom: 60px;">
                  <li style="display: inline-block; height: 12px; line-height: 12px; list-style: none;"><a style="color: #333; font-size: 12px; position: relative; top: -2px; text-decoration: none;" href="#">forgot id/pw?</a></li>  
                </ul>
                <button style="display: block; width: 220px; height: 50px; margin: 0 auto; border: none; background-color: #1673ea; color: #fff; font-size: 14px; cursor: pointer;" type="submit" onclick="location.href='http://127.0.0.1:5500/Smart_DoorLock/manage.html?#';location.href='링크';">Login</button>
              </fieldset>
            </form>
          </div>
    </body> 
</html>
    )rawliteral";
    
String manage_html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
    <body style="background-color: #ededed; padding: 0; margin: 0; font-size: 16px;">
        <form style="width: 100%; height: 450px; background-color: #fff; padding: 60px; box-sizing: border-box;" action="#">
            <div>
                <input type="number" placeholder="비밀번호...">
            </div>
            <div>
                <input type="date">
                <input type="time">
            </div>
            <button type="submit" value="Value">설정</button>
        </form> 
    </body>
</html>
  )rawliteral";

String lcd_html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <div class="input-group mb-3">
        <select class="form-select" id="inputGroupSelect02">
          <option selected>Choose...</option>
          <option value="1">One</option>
          <option value="2">Two</option>
          <option value="3">Three</option>
          <option value="4">Hello</option>
        </select>
        <label class="input-group-text" for="inputGroupSelect02">Options</label>
      </div>
</body>
</html><!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <div class="input-group mb-3">
        <select class="form-select" id="inputGroupSelect02">
          <option selected>Choose...</option>
          <option value="1">One</option>
          <option value="2">Two</option>
          <option value="3">Three</option>
          <option value="4">Hello</option>
        </select>
        <label class="input-group-text" for="inputGroupSelect02">Options</label>
      </div>
</body>
</html>
)rawliteral";
void printLoginPage(WiFiClient* client) {    
    client->println(login_html);
}
void printManagePage(WiFiClient* client) {
    client->println(manage_html);
}
void printLcdPage(WiFiClient* client) {    
    client->println(lcd_html);
}
