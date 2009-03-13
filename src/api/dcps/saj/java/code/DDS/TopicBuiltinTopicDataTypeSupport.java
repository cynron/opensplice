package DDS;

import org.opensplice.dds.dcps.Utilities;

public class TopicBuiltinTopicDataTypeSupport extends org.opensplice.dds.dcps.TypeSupportImpl implements TopicBuiltinTopicDataTypeSupportOperations
{
    private static java.lang.String idl_type_name = "DDS::TopicBuiltinTopicData";
    private static java.lang.String idl_key_list = "key";

    private long copyCache;

    public TopicBuiltinTopicDataTypeSupport()
    {
        super("DDS/TopicBuiltinTopicDataDataReaderImpl",
            "DDS/TopicBuiltinTopicDataDataWriterImpl",
            "(LDDS/TopicBuiltinTopicDataTypeSupport;)V");

        int success = 0;

        try {
            success = org.opensplice.dds.dcps.FooTypeSupportImpl.Alloc(
                    this,
                    idl_type_name,
                    idl_key_list,
                    DDS.TopicBuiltinTopicDataMetaHolder.metaDescriptor);
        } catch (UnsatisfiedLinkError ule) {
            /*
             * JNI library is not loaded if no instance of the
             * DomainParticipantFactory exists.
             */
            DDS.DomainParticipantFactory f = DDS.DomainParticipantFactory.get_instance();
            
            if (f != null) {
                success = org.opensplice.dds.dcps.FooTypeSupportImpl.Alloc(
                        this,
                        idl_type_name,
                        idl_key_list,
                        DDS.TopicBuiltinTopicDataMetaHolder.metaDescriptor);
            }
        }
        if (success == 0) {
            throw Utilities.createException(
                    Utilities.EXCEPTION_TYPE_NO_MEMORY,
                    "Could not allocate TopicBuiltinTopicDataTypeSupport." );
        }
    }

    protected void finalize()
    {
        org.opensplice.dds.dcps.FooTypeSupportImpl.Free(this);
    }

    public long get_copyCache()
    {
        return copyCache;
    }

    public int register_type(
            DDS.DomainParticipant participant,
            java.lang.String type_name)
    {
        return org.opensplice.dds.dcps.FooTypeSupportImpl.registerType(
                this,
                participant,
                type_name);
    }

    public String get_type_name()
    {
        return idl_type_name;
    }

}