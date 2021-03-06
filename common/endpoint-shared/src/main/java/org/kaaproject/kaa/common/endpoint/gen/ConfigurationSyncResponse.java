/**
 * Autogenerated by Avro
 * 
 * DO NOT EDIT DIRECTLY
 */
package org.kaaproject.kaa.common.endpoint.gen;  
@SuppressWarnings("all")
@org.apache.avro.specific.AvroGenerated
public class ConfigurationSyncResponse extends org.apache.avro.specific.SpecificRecordBase implements org.apache.avro.specific.SpecificRecord {
  public static final org.apache.avro.Schema SCHEMA$ = new org.apache.avro.Schema.Parser().parse("{\"type\":\"record\",\"name\":\"ConfigurationSyncResponse\",\"namespace\":\"org.kaaproject.kaa.common.endpoint.gen\",\"fields\":[{\"name\":\"appStateSeqNumber\",\"type\":\"int\"},{\"name\":\"responseStatus\",\"type\":{\"type\":\"enum\",\"name\":\"SyncResponseStatus\",\"symbols\":[\"NO_DELTA\",\"DELTA\",\"RESYNC\"]}},{\"name\":\"confSchemaBody\",\"type\":[\"bytes\",\"null\"]},{\"name\":\"confDeltaBody\",\"type\":[\"bytes\",\"null\"]}],\"direction\":\"in\"}");
  public static org.apache.avro.Schema getClassSchema() { return SCHEMA$; }
   private int appStateSeqNumber;
   private org.kaaproject.kaa.common.endpoint.gen.SyncResponseStatus responseStatus;
   private java.nio.ByteBuffer confSchemaBody;
   private java.nio.ByteBuffer confDeltaBody;

  /**
   * Default constructor.  Note that this does not initialize fields
   * to their default values from the schema.  If that is desired then
   * one should use {@link \#newBuilder()}. 
   */
  public ConfigurationSyncResponse() {}

  /**
   * All-args constructor.
   */
  public ConfigurationSyncResponse(java.lang.Integer appStateSeqNumber, org.kaaproject.kaa.common.endpoint.gen.SyncResponseStatus responseStatus, java.nio.ByteBuffer confSchemaBody, java.nio.ByteBuffer confDeltaBody) {
    this.appStateSeqNumber = appStateSeqNumber;
    this.responseStatus = responseStatus;
    this.confSchemaBody = confSchemaBody;
    this.confDeltaBody = confDeltaBody;
  }

  public org.apache.avro.Schema getSchema() { return SCHEMA$; }
  // Used by DatumWriter.  Applications should not call. 
  public java.lang.Object get(int field$) {
    switch (field$) {
    case 0: return appStateSeqNumber;
    case 1: return responseStatus;
    case 2: return confSchemaBody;
    case 3: return confDeltaBody;
    default: throw new org.apache.avro.AvroRuntimeException("Bad index");
    }
  }
  // Used by DatumReader.  Applications should not call. 
  @SuppressWarnings(value="unchecked")
  public void put(int field$, java.lang.Object value$) {
    switch (field$) {
    case 0: appStateSeqNumber = (java.lang.Integer)value$; break;
    case 1: responseStatus = (org.kaaproject.kaa.common.endpoint.gen.SyncResponseStatus)value$; break;
    case 2: confSchemaBody = (java.nio.ByteBuffer)value$; break;
    case 3: confDeltaBody = (java.nio.ByteBuffer)value$; break;
    default: throw new org.apache.avro.AvroRuntimeException("Bad index");
    }
  }

  /**
   * Gets the value of the 'appStateSeqNumber' field.
   */
  public java.lang.Integer getAppStateSeqNumber() {
    return appStateSeqNumber;
  }

  /**
   * Sets the value of the 'appStateSeqNumber' field.
   * @param value the value to set.
   */
  public void setAppStateSeqNumber(java.lang.Integer value) {
    this.appStateSeqNumber = value;
  }

  /**
   * Gets the value of the 'responseStatus' field.
   */
  public org.kaaproject.kaa.common.endpoint.gen.SyncResponseStatus getResponseStatus() {
    return responseStatus;
  }

  /**
   * Sets the value of the 'responseStatus' field.
   * @param value the value to set.
   */
  public void setResponseStatus(org.kaaproject.kaa.common.endpoint.gen.SyncResponseStatus value) {
    this.responseStatus = value;
  }

  /**
   * Gets the value of the 'confSchemaBody' field.
   */
  public java.nio.ByteBuffer getConfSchemaBody() {
    return confSchemaBody;
  }

  /**
   * Sets the value of the 'confSchemaBody' field.
   * @param value the value to set.
   */
  public void setConfSchemaBody(java.nio.ByteBuffer value) {
    this.confSchemaBody = value;
  }

  /**
   * Gets the value of the 'confDeltaBody' field.
   */
  public java.nio.ByteBuffer getConfDeltaBody() {
    return confDeltaBody;
  }

  /**
   * Sets the value of the 'confDeltaBody' field.
   * @param value the value to set.
   */
  public void setConfDeltaBody(java.nio.ByteBuffer value) {
    this.confDeltaBody = value;
  }

  /** Creates a new ConfigurationSyncResponse RecordBuilder */
  public static org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder newBuilder() {
    return new org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder();
  }
  
  /** Creates a new ConfigurationSyncResponse RecordBuilder by copying an existing Builder */
  public static org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder newBuilder(org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder other) {
    return new org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder(other);
  }
  
  /** Creates a new ConfigurationSyncResponse RecordBuilder by copying an existing ConfigurationSyncResponse instance */
  public static org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder newBuilder(org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse other) {
    return new org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder(other);
  }
  
  /**
   * RecordBuilder for ConfigurationSyncResponse instances.
   */
  public static class Builder extends org.apache.avro.specific.SpecificRecordBuilderBase<ConfigurationSyncResponse>
    implements org.apache.avro.data.RecordBuilder<ConfigurationSyncResponse> {

    private int appStateSeqNumber;
    private org.kaaproject.kaa.common.endpoint.gen.SyncResponseStatus responseStatus;
    private java.nio.ByteBuffer confSchemaBody;
    private java.nio.ByteBuffer confDeltaBody;

    /** Creates a new Builder */
    private Builder() {
      super(org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.SCHEMA$);
    }
    
    /** Creates a Builder by copying an existing Builder */
    private Builder(org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder other) {
      super(other);
      if (isValidValue(fields()[0], other.appStateSeqNumber)) {
        this.appStateSeqNumber = data().deepCopy(fields()[0].schema(), other.appStateSeqNumber);
        fieldSetFlags()[0] = true;
      }
      if (isValidValue(fields()[1], other.responseStatus)) {
        this.responseStatus = data().deepCopy(fields()[1].schema(), other.responseStatus);
        fieldSetFlags()[1] = true;
      }
      if (isValidValue(fields()[2], other.confSchemaBody)) {
        this.confSchemaBody = data().deepCopy(fields()[2].schema(), other.confSchemaBody);
        fieldSetFlags()[2] = true;
      }
      if (isValidValue(fields()[3], other.confDeltaBody)) {
        this.confDeltaBody = data().deepCopy(fields()[3].schema(), other.confDeltaBody);
        fieldSetFlags()[3] = true;
      }
    }
    
    /** Creates a Builder by copying an existing ConfigurationSyncResponse instance */
    private Builder(org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse other) {
            super(org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.SCHEMA$);
      if (isValidValue(fields()[0], other.appStateSeqNumber)) {
        this.appStateSeqNumber = data().deepCopy(fields()[0].schema(), other.appStateSeqNumber);
        fieldSetFlags()[0] = true;
      }
      if (isValidValue(fields()[1], other.responseStatus)) {
        this.responseStatus = data().deepCopy(fields()[1].schema(), other.responseStatus);
        fieldSetFlags()[1] = true;
      }
      if (isValidValue(fields()[2], other.confSchemaBody)) {
        this.confSchemaBody = data().deepCopy(fields()[2].schema(), other.confSchemaBody);
        fieldSetFlags()[2] = true;
      }
      if (isValidValue(fields()[3], other.confDeltaBody)) {
        this.confDeltaBody = data().deepCopy(fields()[3].schema(), other.confDeltaBody);
        fieldSetFlags()[3] = true;
      }
    }

    /** Gets the value of the 'appStateSeqNumber' field */
    public java.lang.Integer getAppStateSeqNumber() {
      return appStateSeqNumber;
    }
    
    /** Sets the value of the 'appStateSeqNumber' field */
    public org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder setAppStateSeqNumber(int value) {
      validate(fields()[0], value);
      this.appStateSeqNumber = value;
      fieldSetFlags()[0] = true;
      return this; 
    }
    
    /** Checks whether the 'appStateSeqNumber' field has been set */
    public boolean hasAppStateSeqNumber() {
      return fieldSetFlags()[0];
    }
    
    /** Clears the value of the 'appStateSeqNumber' field */
    public org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder clearAppStateSeqNumber() {
      fieldSetFlags()[0] = false;
      return this;
    }

    /** Gets the value of the 'responseStatus' field */
    public org.kaaproject.kaa.common.endpoint.gen.SyncResponseStatus getResponseStatus() {
      return responseStatus;
    }
    
    /** Sets the value of the 'responseStatus' field */
    public org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder setResponseStatus(org.kaaproject.kaa.common.endpoint.gen.SyncResponseStatus value) {
      validate(fields()[1], value);
      this.responseStatus = value;
      fieldSetFlags()[1] = true;
      return this; 
    }
    
    /** Checks whether the 'responseStatus' field has been set */
    public boolean hasResponseStatus() {
      return fieldSetFlags()[1];
    }
    
    /** Clears the value of the 'responseStatus' field */
    public org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder clearResponseStatus() {
      responseStatus = null;
      fieldSetFlags()[1] = false;
      return this;
    }

    /** Gets the value of the 'confSchemaBody' field */
    public java.nio.ByteBuffer getConfSchemaBody() {
      return confSchemaBody;
    }
    
    /** Sets the value of the 'confSchemaBody' field */
    public org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder setConfSchemaBody(java.nio.ByteBuffer value) {
      validate(fields()[2], value);
      this.confSchemaBody = value;
      fieldSetFlags()[2] = true;
      return this; 
    }
    
    /** Checks whether the 'confSchemaBody' field has been set */
    public boolean hasConfSchemaBody() {
      return fieldSetFlags()[2];
    }
    
    /** Clears the value of the 'confSchemaBody' field */
    public org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder clearConfSchemaBody() {
      confSchemaBody = null;
      fieldSetFlags()[2] = false;
      return this;
    }

    /** Gets the value of the 'confDeltaBody' field */
    public java.nio.ByteBuffer getConfDeltaBody() {
      return confDeltaBody;
    }
    
    /** Sets the value of the 'confDeltaBody' field */
    public org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder setConfDeltaBody(java.nio.ByteBuffer value) {
      validate(fields()[3], value);
      this.confDeltaBody = value;
      fieldSetFlags()[3] = true;
      return this; 
    }
    
    /** Checks whether the 'confDeltaBody' field has been set */
    public boolean hasConfDeltaBody() {
      return fieldSetFlags()[3];
    }
    
    /** Clears the value of the 'confDeltaBody' field */
    public org.kaaproject.kaa.common.endpoint.gen.ConfigurationSyncResponse.Builder clearConfDeltaBody() {
      confDeltaBody = null;
      fieldSetFlags()[3] = false;
      return this;
    }

    @Override
    public ConfigurationSyncResponse build() {
      try {
        ConfigurationSyncResponse record = new ConfigurationSyncResponse();
        record.appStateSeqNumber = fieldSetFlags()[0] ? this.appStateSeqNumber : (java.lang.Integer) defaultValue(fields()[0]);
        record.responseStatus = fieldSetFlags()[1] ? this.responseStatus : (org.kaaproject.kaa.common.endpoint.gen.SyncResponseStatus) defaultValue(fields()[1]);
        record.confSchemaBody = fieldSetFlags()[2] ? this.confSchemaBody : (java.nio.ByteBuffer) defaultValue(fields()[2]);
        record.confDeltaBody = fieldSetFlags()[3] ? this.confDeltaBody : (java.nio.ByteBuffer) defaultValue(fields()[3]);
        return record;
      } catch (Exception e) {
        throw new org.apache.avro.AvroRuntimeException(e);
      }
    }
  }
}
