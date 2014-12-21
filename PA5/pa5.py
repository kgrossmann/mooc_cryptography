import sys
import math
from oracle import *
from helper import *
from decimal import Decimal
from helper import modinv
# Launches a forgery on a padded-message RSA signature scheme, based on a plain RSA signature
# This is the message to be forded on the oracle
msg = "Crypto is hard --- even schemes that look complex can be broken"     
#
# public RSA key (decimal)
e = 65537
N = 119077393994976313358209514872004186781083638474007212865571534799455802984783764695504518716476645854434703350542987348935664430222174597252144205891641172082602942313168180100366024600206994820541840725743590501646516068078269875871068596540116450747659687492528762004294694507524718065820838211568885027869
# the padding scheme P(m) is defined as P(m) = µ*m + r, where µ is the shifting coefficient, and r is a padding coefficient
# Here, with the padded message M = 0x00 m 0x00 m , r is zero and w = 2**(64*8)+1
mu = 2**(64*8)+1
#
def CalculateMessageValue(message):
    val = 0    
    for ch in message:
       #print(ch)
       val = val<<8
       val = val + ord(ch)    
    return val   
#
def PrintSomethingOutHex(msg,val):
   print(msg + str(":"))
   print(hex(val))
#
print(msg)
m =  CalculateMessageValue(msg)
m1 = 2
m2 = m//2
assert(m1*m2==m)
PrintSomethingOutHex("m",m)
PrintSomethingOutHex("m1",m1)
PrintSomethingOutHex("m2",m2)
assert(N>mu*m1)
assert(N>mu*m2)
assert(N>m1*m2)
# connect to python oracle
Oracle_Connect()
sigma1 = Sign(m1)
assert(sigma1>=0 and sigma1<N)
sigma2 = Sign(m2)
assert(sigma2>=0 and sigma2<N)
sigma_mu = Sign(0x01)
assert(pow(sigma_mu,e,N)==mu)
# After having derived the linear equation for the e-th power of the signature on
# m (sigma(m)^e = sigma(m1)^e*sigma(m2)^e/sigma(1)^e) = mu*m1*m2),
# we verfiy its validity here:
sigma1_e = pow(sigma1,e,N)
sigma2_e = pow(sigma2,e,N)
sigma_mu_e = pow(sigma_mu,e,N)
assert(sigma1_e*sigma2_e//sigma_mu_e==mu*m1*m2)
# if the above is true, we simply can take the e-th root in order to
# obtain the signature on m;
# the e-th root is then [sigma(m1)*sigma(m2)/sigma(1) mod N];
# however, most importantly, note that modolu division by 'b' is done by
# multiplying with the modolu inverse of b, that is b^-1 = modinv(b,N)
# Hence, we have multiply by modinv(sigma_mu,N):
sigma_mu_inv = modinv(sigma_mu,N)
sigma =  (sigma1*sigma2*sigma_mu_inv)%N
PrintSomethingOutHex("sigma",sigma)   
if Verify(m,sigma):
   print("Well done!")
else:     
   print("Try Again!")
#
Oracle_Disconnect()
print("Finished")

