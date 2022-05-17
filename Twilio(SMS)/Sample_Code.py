import json
from twilio.rest import Client 

# get JSON payload with form {"Message" : "asdfasdf"}
def lambda_handler(event, context):
    # TODO implement
    
    ms = event['Message']
    
    account_sid = 'ACce0456681b1d61bc9de849a8e370253e' 
    auth_token = '[AuthToken]' 
    client = Client(account_sid, auth_token) 
 
    message = client.messages.create(
        messaging_service_sid='MG1082d7cd0834adeba6ef73528805bfa9', 
        body=ms,      
        #to='Appropriate Phone Number' 
        ) 
 
    print(message.sid)
    
    return {
        'statusCode': 200,
        'body': json.dumps(ms)
    }
