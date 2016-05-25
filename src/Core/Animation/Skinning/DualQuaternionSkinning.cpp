#include <Core/Animation/Skinning/DualQuaternionSkinning.hpp>
#include <Core/Time/Timer.hpp>

namespace Ra {
namespace Core {
namespace Animation {

void computeDQ( const Pose& pose, const WeightMatrix& weight, DQList& DQ ) {
    DQ.clear();
    DQ.resize( weight.rows(), DualQuaternion( Quaternion( 0, 0, 0, 0 ), Quaternion( 0, 0, 0, 0 ) ) );
    //DualQuaternion* wqMat = new DualQuaternion[weight.rows() * weight.cols()];
    //Ra::Core::Timer::TimePoint t0 = Ra::Core::Timer::Clock::now();
    //#pragma omp parallel for
        for( uint k = 0; k < weight.outerSize(); ++k ) {
            DualQuaternion q( pose[k] );
            for( WeightMatrix::InnerIterator it( weight, k ); it; ++it ) {
                const uint   i = it.row();
                const Scalar w = it.value();
                const auto wq = q * w;
//                wqMat[i * weight.cols() + k] = wq;
//    #pragma omp critical
//                {
                    DQ[i] += wq;
//                }
            }
        }
        //Ra::Core::Timer::TimePoint t1 = Ra::Core::Timer::Clock::now();
    //#pragma omp parallel for
//    for( uint i = 0; i < weight.rows() ; ++i)
//    {
//        for (int j = 0; j < weight.cols(); j++)
//            DQ[i] += wqMat[i * weight.cols() + j];
//    }
//    delete wqMat;
    
   // Normalize all dual quats.
    #pragma omp parallel for
        for( uint i = 0; i < DQ.size() ; ++i) {
            DQ[i].normalize();
        }
        //Ra::Core::Timer::TimePoint t2 = Ra::Core::Timer::Clock::now();
        //std::cout << "Timing: " << Ra::Core::Timer::getIntervalMicro(t0, t1) << " | " << Ra::Core::Timer::getIntervalMicro(t1, t2)  << std::endl;
}

void DualQuaternionSkinning( const Vector3Array& input, const DQList& DQ, Vector3Array& output ) {
    const uint size = input.size();
    output.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        output[i] = DQ[i].transform( input[i] );
    }
}

} // namespace Animation
} // namespace Core
} // namespace Ra
