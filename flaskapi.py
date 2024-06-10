from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime

app = Flask(__name__)

app.config['SQLALCHEMY_DATABASE_URI'] = 'mysql+mysqlconnector://root:@localhost/sensor_data_db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db = SQLAlchemy(app)

class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    temperature = db.Column(db.Float, nullable=False)
    time = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)

    def to_dict(self):
        return {
            'id': self.id,
            'temperature': self.temperature,
            'time': self.time.isoformat()
        }

with app.app_context():
    db.create_all()

@app.route("/sensor/data", methods=["POST", "GET"])
def sensor_data():
    if request.method == "POST":
        data = request.get_json()
        new_data = SensorData(
            temperature=data['temperature'],
            time=datetime.fromisoformat(data['time']) 
        )
        db.session.add(new_data)
        db.session.commit()
        return jsonify(new_data.to_dict()), 201
    else:
        all_data = SensorData.query.all()
        return jsonify([data.to_dict() for data in all_data])

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000, debug=True)
