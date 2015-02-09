/**
 * Autogenerated by Avro
 * 
 * DO NOT EDIT DIRECTLY
 */
package org.kaaproject.kaa.server.verifiers.gplus.config.gen;  
@SuppressWarnings("all")
@org.apache.avro.specific.AvroGenerated
public class GplusAvroConfig extends org.apache.avro.specific.SpecificRecordBase implements org.apache.avro.specific.SpecificRecord {
  public static final org.apache.avro.Schema SCHEMA$ = new org.apache.avro.Schema.Parser().parse("{\"type\":\"record\",\"name\":\"GplusAvroConfig\",\"namespace\":\"org.kaaproject.kaa.server.verifiers.gplus.config.gen\",\"fields\":[{\"name\":\"max_parallel_connections\",\"type\":\"int\",\"displayName\":\"Maximum parallel connections opened\",\"by_default\":\"20\"},{\"name\":\"min_parallel_connections\",\"type\":\"int\",\"displayName\":\"Minimum parallel connections opened\",\"by_default\":\"2\"},{\"name\":\"keep_alive_time_milliseconds\",\"type\":\"long\",\"displayName\":\"Time to keep connection alive\",\"by_default\":\"60_000\"}]}");
  public static org.apache.avro.Schema getClassSchema() { return SCHEMA$; }
   private int max_parallel_connections;
   private int min_parallel_connections;
   private long keep_alive_time_milliseconds;

  /**
   * Default constructor.  Note that this does not initialize fields
   * to their default values from the schema.  If that is desired then
   * one should use {@link \#newBuilder()}. 
   */
  public GplusAvroConfig() {}

  /**
   * All-args constructor.
   */
  public GplusAvroConfig(java.lang.Integer max_parallel_connections, java.lang.Integer min_parallel_connections, java.lang.Long keep_alive_time_milliseconds) {
    this.max_parallel_connections = max_parallel_connections;
    this.min_parallel_connections = min_parallel_connections;
    this.keep_alive_time_milliseconds = keep_alive_time_milliseconds;
  }

  public org.apache.avro.Schema getSchema() { return SCHEMA$; }
  // Used by DatumWriter.  Applications should not call. 
  public java.lang.Object get(int field$) {
    switch (field$) {
    case 0: return max_parallel_connections;
    case 1: return min_parallel_connections;
    case 2: return keep_alive_time_milliseconds;
    default: throw new org.apache.avro.AvroRuntimeException("Bad index");
    }
  }
  // Used by DatumReader.  Applications should not call. 
  @SuppressWarnings(value="unchecked")
  public void put(int field$, java.lang.Object value$) {
    switch (field$) {
    case 0: max_parallel_connections = (java.lang.Integer)value$; break;
    case 1: min_parallel_connections = (java.lang.Integer)value$; break;
    case 2: keep_alive_time_milliseconds = (java.lang.Long)value$; break;
    default: throw new org.apache.avro.AvroRuntimeException("Bad index");
    }
  }

  /**
   * Gets the value of the 'max_parallel_connections' field.
   */
  public java.lang.Integer getMaxParallelConnections() {
    return max_parallel_connections;
  }

  /**
   * Sets the value of the 'max_parallel_connections' field.
   * @param value the value to set.
   */
  public void setMaxParallelConnections(java.lang.Integer value) {
    this.max_parallel_connections = value;
  }

  /**
   * Gets the value of the 'min_parallel_connections' field.
   */
  public java.lang.Integer getMinParallelConnections() {
    return min_parallel_connections;
  }

  /**
   * Sets the value of the 'min_parallel_connections' field.
   * @param value the value to set.
   */
  public void setMinParallelConnections(java.lang.Integer value) {
    this.min_parallel_connections = value;
  }

  /**
   * Gets the value of the 'keep_alive_time_milliseconds' field.
   */
  public java.lang.Long getKeepAliveTimeMilliseconds() {
    return keep_alive_time_milliseconds;
  }

  /**
   * Sets the value of the 'keep_alive_time_milliseconds' field.
   * @param value the value to set.
   */
  public void setKeepAliveTimeMilliseconds(java.lang.Long value) {
    this.keep_alive_time_milliseconds = value;
  }

  /** Creates a new GplusAvroConfig RecordBuilder */
  public static org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder newBuilder() {
    return new org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder();
  }
  
  /** Creates a new GplusAvroConfig RecordBuilder by copying an existing Builder */
  public static org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder newBuilder(org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder other) {
    return new org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder(other);
  }
  
  /** Creates a new GplusAvroConfig RecordBuilder by copying an existing GplusAvroConfig instance */
  public static org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder newBuilder(org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig other) {
    return new org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder(other);
  }
  
  /**
   * RecordBuilder for GplusAvroConfig instances.
   */
  public static class Builder extends org.apache.avro.specific.SpecificRecordBuilderBase<GplusAvroConfig>
    implements org.apache.avro.data.RecordBuilder<GplusAvroConfig> {

    private int max_parallel_connections;
    private int min_parallel_connections;
    private long keep_alive_time_milliseconds;

    /** Creates a new Builder */
    private Builder() {
      super(org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.SCHEMA$);
    }
    
    /** Creates a Builder by copying an existing Builder */
    private Builder(org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder other) {
      super(other);
      if (isValidValue(fields()[0], other.max_parallel_connections)) {
        this.max_parallel_connections = data().deepCopy(fields()[0].schema(), other.max_parallel_connections);
        fieldSetFlags()[0] = true;
      }
      if (isValidValue(fields()[1], other.min_parallel_connections)) {
        this.min_parallel_connections = data().deepCopy(fields()[1].schema(), other.min_parallel_connections);
        fieldSetFlags()[1] = true;
      }
      if (isValidValue(fields()[2], other.keep_alive_time_milliseconds)) {
        this.keep_alive_time_milliseconds = data().deepCopy(fields()[2].schema(), other.keep_alive_time_milliseconds);
        fieldSetFlags()[2] = true;
      }
    }
    
    /** Creates a Builder by copying an existing GplusAvroConfig instance */
    private Builder(org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig other) {
            super(org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.SCHEMA$);
      if (isValidValue(fields()[0], other.max_parallel_connections)) {
        this.max_parallel_connections = data().deepCopy(fields()[0].schema(), other.max_parallel_connections);
        fieldSetFlags()[0] = true;
      }
      if (isValidValue(fields()[1], other.min_parallel_connections)) {
        this.min_parallel_connections = data().deepCopy(fields()[1].schema(), other.min_parallel_connections);
        fieldSetFlags()[1] = true;
      }
      if (isValidValue(fields()[2], other.keep_alive_time_milliseconds)) {
        this.keep_alive_time_milliseconds = data().deepCopy(fields()[2].schema(), other.keep_alive_time_milliseconds);
        fieldSetFlags()[2] = true;
      }
    }

    /** Gets the value of the 'max_parallel_connections' field */
    public java.lang.Integer getMaxParallelConnections() {
      return max_parallel_connections;
    }
    
    /** Sets the value of the 'max_parallel_connections' field */
    public org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder setMaxParallelConnections(int value) {
      validate(fields()[0], value);
      this.max_parallel_connections = value;
      fieldSetFlags()[0] = true;
      return this; 
    }
    
    /** Checks whether the 'max_parallel_connections' field has been set */
    public boolean hasMaxParallelConnections() {
      return fieldSetFlags()[0];
    }
    
    /** Clears the value of the 'max_parallel_connections' field */
    public org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder clearMaxParallelConnections() {
      fieldSetFlags()[0] = false;
      return this;
    }

    /** Gets the value of the 'min_parallel_connections' field */
    public java.lang.Integer getMinParallelConnections() {
      return min_parallel_connections;
    }
    
    /** Sets the value of the 'min_parallel_connections' field */
    public org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder setMinParallelConnections(int value) {
      validate(fields()[1], value);
      this.min_parallel_connections = value;
      fieldSetFlags()[1] = true;
      return this; 
    }
    
    /** Checks whether the 'min_parallel_connections' field has been set */
    public boolean hasMinParallelConnections() {
      return fieldSetFlags()[1];
    }
    
    /** Clears the value of the 'min_parallel_connections' field */
    public org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder clearMinParallelConnections() {
      fieldSetFlags()[1] = false;
      return this;
    }

    /** Gets the value of the 'keep_alive_time_milliseconds' field */
    public java.lang.Long getKeepAliveTimeMilliseconds() {
      return keep_alive_time_milliseconds;
    }
    
    /** Sets the value of the 'keep_alive_time_milliseconds' field */
    public org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder setKeepAliveTimeMilliseconds(long value) {
      validate(fields()[2], value);
      this.keep_alive_time_milliseconds = value;
      fieldSetFlags()[2] = true;
      return this; 
    }
    
    /** Checks whether the 'keep_alive_time_milliseconds' field has been set */
    public boolean hasKeepAliveTimeMilliseconds() {
      return fieldSetFlags()[2];
    }
    
    /** Clears the value of the 'keep_alive_time_milliseconds' field */
    public org.kaaproject.kaa.server.verifiers.gplus.config.gen.GplusAvroConfig.Builder clearKeepAliveTimeMilliseconds() {
      fieldSetFlags()[2] = false;
      return this;
    }

    @Override
    public GplusAvroConfig build() {
      try {
        GplusAvroConfig record = new GplusAvroConfig();
        record.max_parallel_connections = fieldSetFlags()[0] ? this.max_parallel_connections : (java.lang.Integer) defaultValue(fields()[0]);
        record.min_parallel_connections = fieldSetFlags()[1] ? this.min_parallel_connections : (java.lang.Integer) defaultValue(fields()[1]);
        record.keep_alive_time_milliseconds = fieldSetFlags()[2] ? this.keep_alive_time_milliseconds : (java.lang.Long) defaultValue(fields()[2]);
        return record;
      } catch (Exception e) {
        throw new org.apache.avro.AvroRuntimeException(e);
      }
    }
  }
}