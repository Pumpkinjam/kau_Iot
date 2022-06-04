#pragma once
#include <WiFi.h>
#include "time.h"
#include <EEPROM.h>
#define EEPROM_SIZE 200
#define COMMAND_SIZE 300

void printLoginPage(WiFiClient*);
void printManagePage(WiFiClient*);
void printLcdPage(WiFiClient*);

String login_html = R"rawliteral(<!DOCTYPE html>
<html>
<head>
    <meta charset=="UTF-8">
    <title>Document</title>
    <meta name="viewport" content="width=device-width, height=device-height, minimum-scale=1.0, maximum-scale=1.0, initial-scale=1.0"> 
    <style>
        
    </style>
</head>
    <body style="background-color: #ededed; padding: 0; margin: 0;"> 
        <div style="width: 780px; height: 550px; margin: 0 auto;" class="loginbox">
            <h2 style="width: 100%;">Login</h2>
            <form style="width: 100%; height: 450px; background-color: #fff; padding: 60px; box-sizing: border-box;" action="#" method="get">
              <fieldset style="border: none;">
                <legend style="position: absolute; left: -999em;">로그?��</legend>
                <label style="display: block; font-size: 12px; color: #333; margin-bottom: 10px;" for="loginid">id</label>
                <input style="width: 600px; display: block; height: 50px; margin-bottom: 25px; border: none; background-color: #ededed; padding: 0 20px; box-sizing: border-box;" type="text" id="loginid" placeholder="id">
                <label style="display: block; font-size: 12px; color: #333; margin-bottom: 10px;" for="loginpw">password</label>
                <input style="width: 600px; display: block; height: 50px; margin-bottom: 25px; border: none; background-color: #ededed; padding: 0 20px; box-sizing: border-box;" type="password" id="loginpw" placeholder="password">
                <ul style="width: 100%; text-align: right; margin-bottom: 60px;">
                  <li style="display: inline-block; height: 12px; line-height: 12px; list-style: none;"><a style="color: #333; font-size: 12px; position: relative; top: -2px; text-decoration: none;" href="#">forgot id/pw?</a></li>  
                </ul>
                <button style="display: block; width: 220px; height: 50px; margin: 0 auto; border: none; background-color: #1673ea; color: #fff; font-size: 14px; cursor: pointer;" type="button" onclick="tryLogIn();">Login</button>
                </fieldset>
            </form>
          </div>
          <script>
            function tryLogIn() {
                let id = document.getElementById('loginid').value;
                let pw = document.getElementById('loginpw').value;
                if (id == 'admin' && pw == 'alpine') {
                    location.href = '/manage';
                }
                else {
                    alert('Wrong ID or Password');
                }
            }
          </script>
    </body> 
</html>
    )rawliteral";
    
String manage_html = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    
    <title>Document</title>
    <style>
        
    </style>
</head>
    <body style="background-color: #ededed; padding: 0; margin: 0; font-size: 16px;">
        <form style="width: 100%; height: 450px; padding: 60px; box-sizing: border-box;" action="#">
            <div>
                ��???��ȣ : <input id="password" type="number" placeholder="��???��ȣ..." maxlength="8">
            </div>
            <div>
                <input id="date" type="date">
                <input id="time" type="time">
            </div>
            <div id="result1">-</div>
            <div id="result2">-</div>
            <div id="result3">-</div>
            <button id="btn" type="submit" value="Value">submit</button>
        </form> 

        <script>
            document.getElementById("btn").addEventListener('click', submit);
            async function submit(){
                let v = document.getElementById("password").value;
                let d = document.getElementById("date").value;
                let t = document.getElementById("time").value;
                document.getElementById('result1').innerText = v;
                document.getElementById('result2').innerText = d;
                document.getElementById('result3').innerText = t;
                
                console.log("submited");
                alert(t);

                let jsonVar = {"newpw": v, "date": d, "time": t};

                let response = await fetch('/newpw='+v+'&date='+d+'&time='+t+'&lcd=0&tmp=1', {
                    method: 'POST',
                    headers: {
                      "Content-Type": "application/json;charset=UTF-8",
                      },
                    data: JSON.stringify({
                        "title": "asdf",
                        "body": jsonVar,
                    }),
                })
                
                let result = await response.json();
                alert(result.message);
            }
        </script>
    </body>
</html>
  )rawliteral";
String manage2_html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <!-- <script defer src="https://pyscript.net/alpha/pyscript.js"></script>
    <link rel="stylesheet" href="https://pyscript.net/alpha/pyscript.css" />
    <py-env>
        - numpy
        - matplotlib
    </py-env> -->
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
    <div>
        <input type="text" id="phoneInputA" placeholder="input without - " maxlength="11">
    </div>
    <button id="btn" onclick="submit()" class="form-check" type="submit">submit</button>
    
    <script>
        var iptPhoneA = $("#phoneInputA");

        iptPhoneA.on("keyup", function(){
        if ($(this).val().length == 11) {
            iptPhoneB.focus();
        }
        });
        
        
        function submit(){
            let v = document.getElementById("phoneInputA").value;
            alert(v);
        }
    </script>
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
    <style>
       h1 {
         color: blue;
         text-align: center;
       }
       .form-check {
         /* border: 1px solid #cccccc; */
         width: 130px;
         position: relative;
         left: 50%;
       }
    </style>
</head>
<h1>message</h1>
  <body class="form-check" style="background-color: #ededed;">
    <input class="check" type='checkbox'
        name='message' 
        value='1'
        onclick='checkOnlyOne(this)'/> 11
  <br />
  <input class="check" type='checkbox' 
        name='message' 
        value='2' 
        onclick='checkOnlyOne(this)'/> 22
  <br />
  <input class="check" type='checkbox' 
        name='message' 
        value='3' 
        onclick='checkOnlyOne(this)'/> 33
  <br />
  <button id="submitbtn" type="button" class="btn btn-dark">submit</button>

  <script>
    document.getElementById("submitbtn").addEventListener('click', submit);
    
    function checkOnlyOne(element) {
    let selected = ' ';
    const checkboxes 
        = document.getElementsByName("message");
    
    checkboxes.forEach((cb) => {
      cb.checked = false;
      
      selected = cb.value;
    })
    
    element.checked = true;
  }

  async function submit(){
    let selected = ' ';
    const checkboxes 
        = document.getElementsByName("message");
        
    checkboxes.forEach((cb) => {
      if (cb.checked) {selected = cb.value;}
    })

    if (selected == ' ') { alert("Check something!"); }
    else { 
      console.log("submited : " + selected);

      let response = await fetch('/newpw=0&date=0&time=0&lcd='+selected+'&tmp=0', {
        method: 'POST',
        headers: {
          "Content-Type": "application/json;charset=UTF-8",
        },
        data: JSON.stringify({"wt": "h",}),
      })

      let result = await response.json();
      alert(result.message);
    }
  }
  </script>
</body>
</html>
)rawliteral";
