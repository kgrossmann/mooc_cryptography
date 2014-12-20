
#implements global string alignment using Blosum62 scoring matrix
import sys
import math
from oracle import *
from helper import *
from decimal import Decimal
from helper import modinv
#
msg = "Crypto is tard --- even schemes that look complex can be broken"     
#
N = 119077393994976313358209514872004186781083638474007212865571534799455802984783764695504518716476645854434703350542987348935664430222174597252144205891641172082602942313168180100366024600206994820541840725743590501646516068078269875871068596540116450747659687492528762004294694507524718065820838211568885027869
#
#m1 = 52791214
#m2 = 261384543707279175459029780897046032877114755775880594855403034573631897539149827342219236419177692577875490045527809513173209750330706868481057
m1 = 2
m2 = 6899403691571664154700594697854534544733400387361124260729440327213000130107665956643073472360701636452418330174164167480581367498297458532626667516599

mu = 2**(64*8)+1
e = 65537L
def CalculateMessageValue(m):
    val = 0
    for ch in m:        
       val = val<<8
       val = val + ord(ch)
    return val   
    
def CalculateMessageMForEncryption(m_num):
    val = 0L
    val = long(long(m_num)<<(64*8)) + long(m_num)
    return val
#
def FindRootsOfM(M_num):
    i = 2.0
    while(i<100.0):
        root = float(M_num**(1/i))
        if root%1==0:
            if root**i==M_num:
               print str(i) + "," + str(long(root))
        i = i + 1.0        
#
def FindFactorsOfM(N,M_num):
   m1=2
   while(m1<long(2**63-1)):
      fac1 = m1
      if M_num%fac1==0:
         fac2 = M_num/fac1      
         if (fac1*fac2)%N == M_num:
            #print str(fac1) + "," + str(fac2)
            return [fac1,fac2]
      m1 = m1 +1    
#        
def FindLimitOfm(N,M_num,r):
   root_of_M_num = long((M_num)**(1/r))      
   m1_limit = root_of_M_num>>0 #(64*8)
   return m1_limit
#   
bitmask63 = 2**63-1
def FindFactorsOfMFaster(N,M_num):
   global bitmask63 
   r = 22100.0
   while(r>=3.0):
     m1=1
     limit_m1 = FindLimitOfm(N,M_num,r)
     print limit_m1
     print r
     m1=limit_m1-2
     #print bin(limit_m1)
     #print (len(str(bin(limit_m1)))-2)/8
     while(m1<=long(limit_m1)):
        #print m1
        #fac1 = long(long(m1)<<(64*8)) + long(m1)
        fac1 = m1
        if fac1**r ==M_num:
           print str(m1) + "," + str(r)
           return        
        m1 = m1 + 1
     r = r-1    


#print modinv(2,10)               

m =  CalculateMessageValue(msg)
print " "
assert(N>mu)
assert(N>mu*m1)
assert(N>mu*m2)
assert(N>m1*m2)
print " "
factors = FindFactorsOfM(N,m)
m1 = factors[0]
m2 = factors[1]
Oracle_Connect()
#
sigmaok = Sign(m)
print hex(sigmaok)
#
sigma1 = Sign(m1)
if Verify(m1,sigma1):
   print "Well done!"
else:     
   print "Try Again!"
assert(sigma1>=0)
sigma2 = Sign(m2)
assert(sigma2>=0)
assert(sigma2<N)   
if Verify(m2,sigma2):
   print "Well done!"
else:     
   print "Try Again!"
#
sigma_mu = Sign(0x01)
assert(sigma_mu>=0)
assert(sigma_mu<N)   
if Verify(0x01,sigma_mu):
   print "Well done!"
else:     
   print "Try Again!"
#
if (m1*m2==m):
   print "sigma_mu"
   print hex(sigma_mu)
   print "sigma1"
   print hex(sigma1)
   print "sigma2"
   print hex(sigma2)   
   #
   mu_d = modinv(pow(sigma_mu,e,N),N)         
   assert(pow(sigma_mu,e,N)==mu)
   assert(pow(sigma1,e,N)==mu*m1)
   assert(pow(sigma2,e,N)==mu*m2)
   m1_dmu_d = modinv(pow(sigma1,e,N),N)
   m2_dmu_d = modinv(pow(sigma2,e,N),N)      
   sigma = (m1_dmu_d*m2_dmu_d/mu_d)%N
   print "sigma"
   print hex(sigma)   
   if Verify(m,long(sigma)):
       print "Well done!"
   else:     
      print "Try Again!"
   #print hex((sigma1*sigma2)/sigmaok)
Oracle_Disconnect()
print "\nFinished"