import sys
import math
from oracle import *
from helper import *
from decimal import Decimal
from helper import modinv
from array import array
#
msg = "Crypto is tard --- even schemes that look complex can be broken"     
#
# RSA key (decimal)
N = 119077393994976313358209514872004186781083638474007212865571534799455802984783764695504518716476645854434703350542987348935664430222174597252144205891641172082602942313168180100366024600206994820541840725743590501646516068078269875871068596540116450747659687492528762004294694507524718065820838211568885027869
mu = 2**(64*8)+1
mu_d = 19874535673401750358860375992226866550058673723115925724586069258315830127243105011635696270808471170276242080820262917712193571204880456298074088733336817642373927316010920341918209617058434151809115052360556679952222377990743401219680909743849171699910793098918754106104345792138383355901997899976887080251
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
'''
def CalculateMessageMForEncryption(m_num):
    val = 0
    val = (m_num)<<(64*8) + m_num
    return val
#
'''
def FindFactorsOfM(N,m):
   m1=2
   while(m1<(2**3-1)):
      fac1 = m1
      if m%fac1==0:
         fac2 = m/fac1      
         if (fac1*fac2)%N == m:
            print (str(fac1) + "," + str(fac2))
            return array([fac1,fac2])
      m1 = m1 +1    
#
print("Hello")
m =  CalculateMessageValue(msg)
#factors = FindFactorsOfM(N,m)
#m1 = factors[0]
#m2 = factors[1]
m1 = 2
m2 = m//2
assert(m1*m2==m)
print(hex(m))
print(hex(m1))
print(hex(m2))
print(" ")
assert(N>mu)
assert(N>mu*m1)
assert(N>mu*m2)
assert(N>m1*m2)
assert(N>mu*m1*m2)
print(" ")
Oracle_Connect()
#
sigmaok = Sign(m)
print(hex(sigmaok))
#
sigma1 = Sign(m1)
if Verify(m1,sigma1):
   print ("Well done!")
else:     
   print("Try Again!")
assert(sigma1>=0)
sigma2 = Sign(m2)
assert(sigma2>=0)
assert(sigma2<N)   
if Verify(m2,sigma2):
   print("Well done!")
else:     
   print("Try Again!")
#
sigma_mu = Sign(0x01)
assert(sigma_mu>=0)
assert(sigma_mu<N)   
assert(pow(sigma_mu,e,N)==mu)
if Verify(0x01,sigma_mu):
   print("Well done!")
else:     
   print("Try Again!")
#
if (m1*m2==m):
   # 
   print("sigma_mu")
   print(hex(sigma_mu))
   print("sigma1")
   print(hex(sigma1))
   print("sigma2")
   print(hex(sigma2))
   #
   sigma1_e = pow(sigma1,e,N)
   sigma2_e = pow(sigma2,e,N)
   sigmaok_e = pow(sigmaok,e,N)   
   '''        
   i = 0
   while(i<=2**64):
      mu_d = sigma_mu + i*N
      if mu_d==mu_d_ok:
         sigma = sigma1/mu_d*sigma2
         print "sigma"
         print hex(sigma)   
         if Verify(m,long(sigma)):
            print "Well done!"
            print i
            print mu_d
            break
         else:     
            print "Try Again!"   
      i = i + 1      
   '''
   sigma = (sigma1*sigma2//sigma_mu)%N
   print("sigma")
   print(hex(sigma))   
   if Verify(m,sigma):
       print("Well done!")
   else:     
       print("Try Again!")
   #    
   #print float(sigma)   
   #print float(sigmaok)   
   #print float(sigmaok/sigma)
Oracle_Disconnect()
print("\nFinished")
