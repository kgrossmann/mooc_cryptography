import sys
import math
from oracle import *
from helper import *
from decimal import Decimal
from helper import modinv
#
msg = "Crypto is hard --- even schemes that look complex can be broken"     
#
# RSA key (decimal)
N = 119077393994976313358209514872004186781083638474007212865571534799455802984783764695504518716476645854434703350542987348935664430222174597252144205891641172082602942313168180100366024600206994820541840725743590501646516068078269875871068596540116450747659687492528762004294694507524718065820838211568885027869
mu = 2**(64*8)+1
e = 65537
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
def PrintSomethingOutDec(msg,val):
   print(msg + str(":"))
   print(val)
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
#
Oracle_Connect()
#sigmaok = Sign(m)
#assert(sigmaok>=0 and sigmaok<N)
#PrintSomethingOutHex("sigmaok",sigmaok)
sigma1 = Sign(m1)
assert(sigma1>=0 and sigma1<N)
PrintSomethingOutHex("sigma1",sigma1)
sigma2 = Sign(m2)
assert(sigma2>=0 and sigma2<N)
PrintSomethingOutHex("sigma2",sigma2)
sigma_mu = Sign(0x01)
assert(pow(sigma_mu,e,N)==mu)
PrintSomethingOutHex("sigma_mu",sigma_mu)
#
sigma1_e = pow(sigma1,e,N)
sigma2_e = pow(sigma2,e,N)
sigma_mu_e = pow(sigma_mu,e,N)
sigma_e = sigma1_e*sigma2_e//sigma_mu_e
PrintSomethingOutHex("sigma_e",sigma_e)
assert(sigma_e==mu*m1*m2)
#if the above is true, can we not derive the following?
sigma_mu_inv = modinv(sigma_mu,N)
sigma =  (sigma1*sigma2*sigma_mu_inv)%N
assert(sigma<N)
PrintSomethingOutHex("sigma",sigma)   
#PrintSomethingOutDec("sigmaok/sigma",Decimal(sigmaok/sigma))
#
if Verify(m,sigma):
   print("Well done!")
else:     
   print("Try Again!")
#
Oracle_Disconnect()
print("Finished")
