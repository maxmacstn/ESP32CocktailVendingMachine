const functions = require('firebase-functions');

// The Firebase Admin SDK to access the Firebase Realtime Database.
const admin = require('firebase-admin');
admin.initializeApp();


// Create and Deploy Your First Cloud Functions
// https://firebase.google.com/docs/functions/write-firebase-functions

exports.helloWorld = functions.https.onRequest((request, response) => {
  response.send("Hello from Firebase!");
});


exports.qrwebhook = functions.https.onRequest((req, res) => {
  // Grab the text parameter.
  const body = req.body;
  // Push the new message into the Realtime Database using the Firebase Admin SDK.
  return admin.database().ref('/transaction/' + body.referenceNo).update(body).then((snapshot) => {

    // Return success
    res.status(200);
    return res.send("ok");
  });
});

exports.getProduct = functions.https.onRequest((req, res) => {
  admin.database().ref('/product').once('value', function(snapshot){
     const data = []
     data.push(snapshot)
     res.status(200).send(data)
  })
  
});

exports.getTransactionNo = functions.https.onRequest((req,res) => {
  admin.database().ref('/service').once('value', function(snapshot){
     const data = [] 
     const date = new Date()
     const transactionNo = String(date.getFullYear()) +  String((date.getMonth() + 1)) + String(date.getDate()) + pad(snapshot.val()['transactionCount'] ,4);
     data.push({"transactionNo" :   transactionNo})
     res.status(200).send(data)

     
    admin.database().ref('/service/transactionCount').set(snapshot.val()['transactionCount'] + 1)


  })
  
});

function pad(num, len) {
  return Array(len + 1 - num.toString().length).join('0') + num;
}

