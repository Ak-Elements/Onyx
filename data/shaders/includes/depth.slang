
import includes.viewconstants;

float linearDepth( float depthSample, float near, float far ) {
    float linear = near * far / ( far + depthSample * ( near - far ) );
    return linear;
}

float linearDepth( float depthSample, in ViewConstants constants ) {
    float linear = constants.Near * constants.Far /
                   ( constants.Far + depthSample * ( constants.Near - constants.Far ) );
    return linear;
}
