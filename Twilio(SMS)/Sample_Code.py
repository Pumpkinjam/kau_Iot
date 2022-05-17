import json
from twilio.rest import Client 

# You need to set 'account_sid', 'messaging_service_sid', phone number
# get JSON payload with form {"Message" : "asdfasdf"}
def lambda_handler(event, context):
    # TODO implement
    
    ms = event['Message']
    
    #account_sid = 'My Account sid' 
    auth_token = '[AuthToken]' 
    client = Client(account_sid, auth_token) 
 
    message = client.messages.create(
        #messaging_service_sid='My Service sid', 
        body=ms,      
        #to='Appropriate Phone Number' 
        ) 
 
    print(message.sid)
    
    return {
        'statusCode': 200,
        'body': json.dumps(ms)
    }
