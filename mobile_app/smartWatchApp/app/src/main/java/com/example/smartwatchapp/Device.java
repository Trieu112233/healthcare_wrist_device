package com.example.smartwatchapp;

public class Device {
    private String id;
    private String name;
    private boolean alert;
    private String alert_type;
    private Object last_alert_time;

    public Device(){}

    public String getId() { return id; }

    public String getName() { return name; }
    public boolean isAlert() { return alert; }
    public String getAlert_type() { return alert_type; }
    public String getLast_alert_time() {
        if (last_alert_time == null) return "N/A";
        return last_alert_time.toString();
    }

    public void setId(String id) { this.id = id; }

    public void setAlert(boolean alert) {
        this.alert = alert;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setAlert_type(String alert_type) {
        this.alert_type = alert_type;
    }

    public void setLast_alert_time(Object last_alert_time) {
        this.last_alert_time = last_alert_time;
    }
}
