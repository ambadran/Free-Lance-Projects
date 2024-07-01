import firebase_admin
from firebase_admin import credentials, firestore
cred = credentials.Certificate(r"graduation-f2c3d-firebase-adminsdk-ov1ww-0967297188.json")
firebase_admin.initialize_app(cred)
db=firestore.client()
DeviceID="abc123"

#Variables from hardware:
hr=89
spo2=100
x,y,z=0.6,0.4,0.7
lat=37.7749
long=-1222.4194
#____________________________________________

#real time metrics collection
realtimemetrics=db.collection('realTimeMetrics').where('deviceId','==',DeviceID).get()
for metric in realtimemetrics:
    id=metric.id
realtimemetrics=db.collection('realTimeMetrics').document(id)

dailyhealth=db.collection('dailyHealth').where('deviceID','==',DeviceID).get()
for health in dailyhealth:
    id=health.id
    dailyhealth_data=health.to_dict()
dailyhealth=db.collection('dailyHealth').document(id)

def realtimemetricsUpdate():
    data={
        'hr':hr,
        'spo2':spo2,
        'x':x,
        'y':y,
        'z':z
    }
    realtimemetrics.update(data)

realtimemetricsUpdate()

def dailyHealthUpdate():
    HR=dailyhealth_data.get('HR')
    HR.append(hr)
    SPO2=dailyhealth_data.get('SPO2')
    SPO2.append(spo2)
    data={
        'HR':HR,
        'SPO2':SPO2
    }
    dailyhealth.update(data)

dailyHealthUpdate()
