#pragma once
#include <WiFi.h>
#include "time.h"
//#include <EEPROM.h>
#define EEPROM_SIZE 200
#define COMMAND_SIZE 300

String main_html = R"rawliteral(<!DOCTYPE html>
<head>
    <meta charset="UTF-8" http-equiv="X-UA-Compatible">
    
    <title>Document</title>
    <style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
        .button { background-color: #3cff00;border: none; color: white;
            text-decoration: none; font-size: 50px; margin: 2px; cursor: pointer;}
        .button2 {background-color: #c278ec;border: none; color: white;
            text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; height: 100px; width: 47%;}
    </style>
</head>
    <body style="background-color: #ededed; padding: 0; margin: 0; font-size: 16px;">
        <div><button class="button" id="btn_open" style="height: 120px; width: 95%;">Open</button></div>
        <div>
            <button class="button2" id="btn_manage">pw change</button>
            <button class="button2" id="btn_manage2">add admin</button>
        </div>
        <div>
            <button class="button2" id="btn_lcd">image & message</button>
            <button class="button2" id="btn_logout">logout</button>
        </div>
    </body>
    <script>
        document.getElementById("btn_open").addEventListener("click", function(){location.href='/open'});
        document.getElementById("btn_manage").addEventListener("click", function(){location.href='/manage'});
        document.getElementById("btn_manage2").addEventListener("click", function(){location.href='/manage2'});
        document.getElementById("btn_lcd").addEventListener("click", function(){location.href='/lcd'});
        document.getElementById("btn_logout").addEventListener("click", function(){location.href='/'});
    </script>
</html>)rawliteral";

String login_html = R"rawliteral(<!DOCTYPE html>
<html>
<head>
    <meta charset=="UTF-8">
    <meta name="viewport" content="width=device-width, height=device-height, minimum-scale=1.0, maximum-scale=1.0, initial-scale=1.0"> 
    <style>
        
    </style>
</head>
    <body style="background-color: #ededed; padding: 0; margin: 0;"> 
        <div style="width: 70%; height: 40%; margin: 0 auto;">
            <h2 style="width: 100%;">Login</h2>
            <form style="width: 100%; height: 100%; background-color: #fff; padding: 60px; box-sizing: border-box;" action="#" method="get">
              <fieldset style="border: none;">
                <label style="display: block; font-size: 12px; color: #333; margin-bottom: 10px;" for="loginid">id</label>
                <input style="width: 100%; display: block; height: 50px; margin-bottom: 25px; border: none; background-color: #ededed; padding: 0 20px; box-sizing: border-box;" type="text" id="loginid" placeholder="id">
                <label style="display: block; font-size: 12px; color: #333; margin-bottom: 10px;" for="loginpw">password</label>
                <input style="width: 100%; display: block; height: 50px; margin-bottom: 25px; border: none; background-color: #ededed; padding: 0 20px; box-sizing: border-box;" type="password" id="loginpw" placeholder="password">
                <ul style="width: 100%; text-align: right; margin-bottom: 60px;">
                  <li style="display: inline-block; height: 12px; line-height: 12px; list-style: none;"><a style="color: #333; font-size: 12px; position: relative; top: -2px; text-decoration: none;" href="#" onclick="do_it_yourself();">forgot id/pw?</a></li>  
                </ul>
                <button style="display: block; width: 50%; height: 50px; margin: 0 auto; border: none; background-color: #1673ea; color: #fff; font-size: 14px; cursor: pointer;" type="button" onclick="tryLogIn();">Login</button>
              </fieldset>
            </form>
        </div>
          <script>
            function do_it_yourself(){alert("Please contact to manager : 010-8138-9633");}
            function tryLogIn(){
                let id = document.getElementById('loginid').value;
                let pw = document.getElementById('loginpw').value;
                if (id == 'admin' && pw == 'alpine') {location.href = '/main';}
                else {alert('Wrong ID or Password');}
            }
          </script>
    </body> 
</html>)rawliteral";
    
String manage_html = R"rawliteral(<!DOCTYPE html>
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
</head>
    <body style="background-color: #ededed; padding: 0; margin: 0; font-size: 16px;">
        <button onclick="location.href='/main';" style="background-color: #a5a5a5; font-size: 10px; border: none; padding: 8px 20px; text-decoration: none; margin: 2px;"><-</button>
        <form style="width: 100%; height: 450px; padding: 60px; box-sizing: border-box;" action="#">
            <div>
                new password : <input id="password" type="number" placeholder="input new password..." maxlength="8">
            </div>
            <div>
                <input id="date" type="date">
                <input id="time" type="time">
            </div>
            <button id="btn" type="submit" value="Value">submit</button>
        </form>
        <script>
            document.getElementById("btn").addEventListener('click', submit);
            async function submit(){
                let v = document.getElementById("password").value;
                let d = document.getElementById("date").value;
                let t = document.getElementById("time").value;

                if (d == '' || t == '') {
                    let response = await fetch('/newpw='+v+'&date='+d+'&time='+t+'&lcd=0&tmp=1', {
                        method: 'POST',
                        headers: {
                        "Content-Type": "application/json;charset=UTF-8",
                        },
                        data: JSON.stringify({
                            "y": "k"
                        }),
                    })
                }
                else {
                    let response = await fetch('/newpw='+v+'&date='+d+'&time='+t+'&lcd=0&tmp=2', {
                        method: 'POST',
                        headers: {
                        "Content-Type": "application/json;charset=UTF-8",
                        },
                        data: JSON.stringify({
                            "y": "k"
                        }),
                    })
                }
                let result = await response.json();
                alert(result.message);
                alert("Set!");
            }
</script></body></html>)rawliteral";
String manage2_html = R"rawliteral(
<!DOCTYPE html>
<html lang="ko">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
    div {
  padding: 2em;
    }
    input{
        width: 200px;
        font-size: 16px;
        text-align: center;
    }
    </style>
</head>
<body style="background-color: #ededed; padding: 0; margin: 0;">
    <button onclick="location.href='/main';" style="background-color: #a5a5a5; font-size: 10px; border: none; padding: 8px 20px; text-decoration: none; margin: 2px;"><-</button>
    <div>
        <input type="text" id="phoneInputA" placeholder="input without - " maxlength="11">
    </div>
    <button id="btn" onclick="submit()" class="form-check" type="submit">submit</button>
    
    <script>
        document.getElementById("btn").addEventListener('click', submit);

        var iptPhoneA = $("#phoneInputA");

        iptPhoneA.on("keyup", function(){
        if ($(this).val().length == 11) {
            iptPhoneB.focus();
        }
        });
        
        async function submit(){
            let v = document.getElementById("phoneInputA").value;
            let response = await fetch('/newnum=' + v, {
            method: 'POST',
            headers: {
            "Content-Type": "application/json;charset=UTF-8",
            },
            data: '{"y":"k"}',
            })
        }
    </script></body></html>)rawliteral";
String lcd_html = R"rawliteral(<!DOCTYPE html><head>
  <meta charset="UTF-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Document</title>
  <style>
     h1 {
       color: blue;
       text-align: center;
     }
  </style>
</head>
<body style="width:max-content" style="background-color: #ededed;">
  <div text-align="left"; style="width: max-content">
      <button onclick="location.href='/main';" style="background-color: #a5a5a5; font-size: 10px; border: none; padding: 8px 20px; text-decoration: none; margin: 2px; float: left;"><-</button>
  </div>
  <div>
   <img width=50% height=20% style="display: block;-webkit-user-select: none;margin: auto;background-color: hsl(0, 0%, 90%);transition: background-color 300ms;" src="http://192.168.37.217/view_image">
  </div>
  
  
  <div><input type="radio" id= "1" name="chk_info" value="I'm out of home. Sorry.">I'm out of home. Sorry.</div>
  <div><input type="radio" id= "2" name="chk_info" value="Please put in front of door.">Please put in front of door.</div>
  <div><input type="radio" id= "3" name="chk_info" value="Contact 010-8138-9633">Contact 010-8138-9633</div>
  <div>
      <input type="radio" id= "4" name="chk_info" value="Custom">
      <input type="text" id="lcdInput" placeholder="message for lcd" maxlength="32">
  </div>
  <button id="btn" onclick="submit()" class="form-check" type="submit">submit</button>
  
  <script>

  document.getElementById("submitbtn").addEventListener('click', submit);

  async function submit(){
    if (document.getElementById('4').checked == true) {
        let msg = decodeURI(document.getElementById("lcdInput").value);
        if (msg == '') { alert("Write something!"); }
        else { 
          console.log("submited : " + msg);
          let response = await fetch('/newpw=0&date=0&time=0&lcd="'+msg+'"&tmp=0', {
          method: 'POST',
          headers: {
          "Content-Type": "application/json;charset=UTF-8",
          },
          data: '{"y":"k"}',
          })
        }
    }
    else {
      var msg = "";
      if (document.getElementById('1').checked == true) {msg = "I'm out of home. Sorry.";}
      else if (document.getElementById('2').checked == true) {msg = "Please put in front of the door.";}
      else if (document.getElementById('3').checked == true) {msg = "Contact 010-8138-9633";}
      else {msg = 'what the';}
      console.log("submited : " + msg);
      let response = await fetch('/newpw=0&date=0&time=0&lcd="'+msg+'"&tmp=0', {
        method: 'POST',
        headers: {
        "Content-Type": "application/json;charset=UTF-8",
        },
        data: '{"y":"k"}',
      })
    }

    let result = await response.json();
    alert(result.message);
  }</script></body></html>)rawliteral";
