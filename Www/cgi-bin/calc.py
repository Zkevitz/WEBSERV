#!/usr/bin/env python3
# coding=utf-8
import os
import time
import  urllib.parse


# while True :
#    time.sleep(1)   
 
# le header est genere dans le code du serveur
# print("Content-Type: text/html\r\n\r\n")
# print("Cache-Control: no-store, no-cache, must-revalidate, max-age=0")
# print("Pragma: no-cache")
# while True:
#     print("")
#  bizarre))
query_string = os.environ.get("QUERY_STRING", "")
params = urllib.parse.parse_qs(query_string)
print("<html><body>")
print("<h1>Parametres recus (GET) :</h1>")
print("dic len = ", len(params))
if params:
    for key, value in params.items():
        print("<p>{} = {}</p>".format(key, value[0]))
else:
    print("<p>Aucun parametre reÇu.</p>")

print("</body></html>")