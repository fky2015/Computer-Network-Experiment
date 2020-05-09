import random
import hashlib


password = input("Please input the password:")
print("The password is:{}".format(password))

challenge = int(100000000 * random.random())
print("The random number is:{}".format(challenge))

string = password + str(challenge)

md5 = hashlib.md5()
md5.update(string.encode("utf-8"))
digest = md5.hexdigest()
print("The md5 digest is:{}".format(digest))
