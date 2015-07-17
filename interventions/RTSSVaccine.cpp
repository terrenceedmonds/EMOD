/***************************************************************************************************

Copyright (c) 2015 Intellectual Ventures Property Holdings, LLC (IVPH) All rights reserved.

EMOD is licensed under the Creative Commons Attribution-Noncommercial-ShareAlike 4.0 License.
To view a copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode.

***************************************************************************************************/

#include "stdafx.h"
#include "RTSSVaccine.h"

#include "Contexts.h"
#include "InterventionEnums.h"
#include "InterventionFactory.h"
#include "MalariaContexts.h"     // for BoostAntibody method

static const char * _module = "RTSSVaccine";

namespace Kernel
{
    BEGIN_QUERY_INTERFACE_BODY(RTSSVaccine)
        HANDLE_INTERFACE(IConfigurable)
        HANDLE_INTERFACE(IDistributableIntervention)
        HANDLE_ISUPPORTS_VIA(IDistributableIntervention)
    END_QUERY_INTERFACE_BODY(RTSSVaccine)

    IMPLEMENT_FACTORY_REGISTERED(RTSSVaccine)
    
    RTSSVaccine::RTSSVaccine()
    {
        initSimTypes( 1, "MALARIA_SIM" );
        initConfigTypeMap( "Antibody_Variant", &antibody_variant, RV_Antibody_Variant_DESC_TEXT, 0, 1e5, 0 );
        initConfigTypeMap( "Boosted_Antibody_Concentration", &boosted_antibody_concentration, RV_Boosted_Antibody_Concentration_DESC_TEXT, 0.0, FLT_MAX, 1.0 );
        initConfigTypeMap( "Cost_To_Consumer", &cost_per_unit, RV_Cost_To_Consumer_DESC_TEXT, 0, 999999, 3.75 );
    }

    bool
    RTSSVaccine::Configure(
        const Configuration * inputJson
    )
    {
        initConfig( "Antibody_Type", antibody_type, inputJson, MetadataDescriptor::Enum("Antibody_Type", RV_Antibody_Type_DESC_TEXT, MDD_ENUM_ARGS(MalariaAntibodyType)) );
        return JsonConfigurable::Configure( inputJson );
    }

    bool
    RTSSVaccine::Distribute(
        IIndividualHumanInterventionsContext *context,
        ICampaignCostObserver * const pCCO
    )
    {
        IMalariaHumanContext * imhc = NULL;
        if (s_OK != context->GetParent()->QueryInterface(GET_IID(IMalariaHumanContext), (void**)&imhc) )
        {
            throw QueryInterfaceException( __FILE__, __LINE__, __FUNCTION__, "imhc", "IMalariaHumanContext", "IIndividualHumanContext" );
        }

        imhc->GetMalariaSusceptibilityContext()->BoostAntibody( antibody_type, antibody_variant, boosted_antibody_concentration );

        return BaseIntervention::Distribute( context, pCCO );
    }

    void RTSSVaccine::Update( float dt )
    {
        // Nothing to do for this intervention, which doesn't have ongoing effects after an initial boosting
    }

    void RTSSVaccine::SetContextTo(
        IIndividualHumanContext *context
    )
    {
        // Nothing to do for this intervention, which doesn't have ongoing effects after an initial boosting
    }
}

#if USE_BOOST_SERIALIZATION || USE_BOOST_MPI
BOOST_CLASS_EXPORT(Kernel::RTSSVaccine)

namespace Kernel {
    template<class Archive>
    void serialize(Archive &ar, RTSSVaccine& obj, const unsigned int v)
    {
        boost::serialization::void_cast_register<RTSSVaccine, IDistributableIntervention>();
        ar & obj.antibody_type;
        ar & obj.antibody_variant;
        ar & obj.boosted_antibody_concentration;
        ar & boost::serialization::base_object<Kernel::BaseIntervention>(obj);
    }
}
#endif