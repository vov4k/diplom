import time
import serial
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import smtplib
import imaplib
import email
import random
 
msg = MIMEMultipart()
TO = 'smart.house.diplom@yandex.ru'
GMAIL_USER = 'smart.house.diplom@gmail.com'
GMAIL_PASS = 'LJeTI2021'


password = GMAIL_PASS
msg['From'] = GMAIL_USER
msg['To'] = TO
msg['Subject'] = "Пожар!"
server = smtplib.SMTP('smtp.gmail.com: 587')
server.starttls()
server.login(msg['From'], password)
ser = serial.Serial('COM3', 9600)

mail = imaplib.IMAP4_SSL('imap.gmail.com')
mail.login(GMAIL_USER, GMAIL_PASS)

flag = 0
timing = time.time()
numkl = random.randint(100, 999)
print(numkl)
while True:


    if flag == 0:
        
        message_ser = str(ser.readline())
        print(message_ser)
        if message_ser[2] == 'F' :
            msg = MIMEMultipart()
            msg['From'] = GMAIL_USER
            msg['To'] = TO
            msg['Subject'] = "Пожар!"
            num = random.randint(100, 999)
            message = 'Датчик обнаружил возгорание! Если Вы наблюдате пожар, оправте в ответ этот код: '+str(num)
            msg.attach(MIMEText(message, 'plain'))
            server.sendmail(msg['From'], msg['To'], msg.as_string())
            print("successfully sent email to:"+  msg['To'])
            timing = time.time()
            flag = 1
        if message_ser[2] == 'H' :
            msg = MIMEMultipart()
            msg['From'] = GMAIL_USER
            msg['To'] = TO
            msg['Subject'] = "Взлом!"
            num = random.randint(100, 999)
            message = 'Произошла попытка взлома двери!'
            msg.attach(MIMEText(message, 'plain'))
            server.sendmail(msg['From'], msg['To'], msg.as_string())
            print("successfully sent email to:"+  msg['To'])
        if message_ser[2] == 'A' :
            msg = MIMEMultipart()
            msg['From'] = GMAIL_USER
            msg['To'] = TO
            msg['Subject'] = "Климат"
            num = random.randint(100, 999)
            message = 'Температура: Влажность:'+message_ser
            msg.attach(MIMEText(message, 'plain'))
            server.sendmail(msg['From'], msg['To'], msg.as_string())
            print("successfully sent email to:"+  msg['To'])
    elif flag == 1:
         
         
        mail.list()
        mail.select("inbox")
        result, data = mail.search(None, "ALL")

        ids = data[0]
        id_list = ids.split()
        latest_email_id = id_list[-1]
         
        result, data = mail.fetch(latest_email_id, "(RFC822)")
        raw_email = data[0][1]
        raw_email_string = raw_email.decode('utf-8')
        email_message = email.message_from_string(raw_email_string)
        body = email_message.get_payload()
        if int(body[:3]) == num: #подтвердили
            flag = 3
        else:
            if time.time() - timing > 50.0:
                flag = 2
    elif flag == 2:
        
        new_mes = float(message_ser.split(":")[1].split("\\")[0])
        if new_mes >= 20.0:
            msg = MIMEMultipart()
            msg['From'] = GMAIL_USER
            msg['To'] = TO
            msg['Subject'] = "Пожар!"
            num = random.randint(100, 999)
            message = 'Дома поднилась слишком высокая температура! Вы не ответели на певрое уведомление. Если Вы НЕ наблюдаете пожар, оправте в ответ этот код: '+str(num)+', в противном случае будут вызваны пожарные.'
            msg.attach(MIMEText(message, 'plain'))
            server.sendmail(msg['From'], msg['To'], msg.as_string())
            print("successfully sent email to:"+  msg['To'])
            timing = time.time()
            flag = 4
        else:
            flag = 0
    elif flag == 3:
        
        ser.write('CONFIRM'.encode('utf-8'))
        flag = 0
        timing = time.time()
    elif flag == 4:
        
    
        mail.list()
        mail.select("inbox")
        result, data = mail.search(None, "ALL")

        ids = data[0]
        id_list = ids.split()
        latest_email_id = id_list[-1]
         
        result, data = mail.fetch(latest_email_id, "(RFC822)")
        raw_email = data[0][1]
        raw_email_string = raw_email.decode('utf-8')
        email_message = email.message_from_string(raw_email_string)
        body = email_message.get_payload()
        if int(body[:3]) == num: #подтвердили
            flag = 0
        else:
            if time.time() - timing > 50.0:
                flag = 3
    time.sleep(0.5) 
server.quit()
