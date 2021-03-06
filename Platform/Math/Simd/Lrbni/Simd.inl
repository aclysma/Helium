#if HELIUM_SIMD_LRBNI

namespace Helium
{
    namespace Simd
    {
        // Type punning between prototype definitions of __m512 and __m512i.
        union _Lrbni512Pun
        {
            __m512 f;
            __m512i i;
        };
    }
}

/// Load a SIMD vector from aligned memory.
///
/// @param[in] pSource  Memory, aligned to HELIUM_SIMD_ALIGNMENT, from which to load.
///
/// @return  SIMD vector.
Helium::Simd::Register Helium::Simd::LoadAligned( const void* pSource )
{
    // LRBni prototype load functions are not const-correct, so we need to const cast here.
    return _mm512_loadd( const_cast< void* >( pSource ), _MM_FULLUPC_NONE, _MM_BROADCAST_16X16, _MM_HINT_NONE );
}

/// Load a SIMD vector from unaligned memory.
///
/// @param[in] pSource  Memory from which to load.
///
/// @return  SIMD vector.
Helium::Simd::Register Helium::Simd::LoadUnaligned( const void* pSource )
{
    // LRBni prototype load functions are not const-correct, so we need to const cast here.
    return _mm512_loadd( const_cast< void* >( pSource ), _MM_FULLUPC_NONE, _MM_BROADCAST_16X16, _MM_HINT_NONE );
}

/// Store the contents of a SIMD vector in aligned memory.
///
/// @param[out] pDest  Memory, aligned to HELIUM_SIMD_ALIGNMENT, in which to store the data.
/// @param[in]  vec    SIMD vector to store.
void Helium::Simd::StoreAligned( void* pDest, Helium::Simd::Register vec )
{
    _mm512_stored( pDest, vec, _MM_DOWNC_NONE, _MM_SUBSET32_16, _MM_HINT_NONE );
}

/// Store the contents of a SIMD vector in unaligned memory.
///
/// @param[out] pDest  Memory in which to store the data.
/// @param[in]  vec    SIMD vector to store.
void Helium::Simd::StoreUnaligned( void* pDest, Helium::Simd::Register vec )
{
    _mm512_stored( pDest, vec, _MM_DOWNC_NONE, _MM_SUBSET32_16, _MM_HINT_NONE );
}

/// Load a 32-bit value into each component of a SIMD vector.
///
/// @param[in] pSource  Address of the 32-bit value to load (must be aligned to a 4-byte boundary).
///
/// @return  SIMD vector.
///
/// @see Store32(), LoadSplat128()
Helium::Simd::Register Helium::Simd::LoadSplat32( const void* pSource )
{
    // LRBni prototype load functions are not const-correct, so we need to const cast here.
    return _mm512_loadd( const_cast< void* >( pSource ), _MM_FULLUPC_NONE, _MM_BROADCAST_1X16, _MM_HINT_NONE );
}

/// Load 16 bytes of data into a SIMD vector, repeating the data as necessary to fill.
///
/// For platforms with only 16-byte SIMD vectors, this has the same effect as LoadAligned().
///
/// @param[in] pSource  Address of the data to load (must be aligned to a 16-byte boundary).
///
/// @return  SIMD vector.
///
/// @see Store128(), LoadSplat32()
Helium::Simd::Register Helium::Simd::LoadSplat128( const void* pSource )
{
    // LRBni prototype load functions are not const-correct, so we need to const cast here.
    return _mm512_loadd( const_cast< void* >( pSource ), _MM_FULLUPC_NONE, _MM_BROADCAST_4X16, _MM_HINT_NONE );
}

/// Store the first 32-bit value of a SIMD vector into memory.
///
/// @param[in] pDest  Address in which to store the value (must be aligned to a 4-byte boundary).
/// @param[in] vec    Vector containing the value to store.
///
/// @see LoadSplat32(), Store128()
void Helium::Simd::Store32( void* pDest, Helium::Simd::Register vec )
{
    _mm512_stored( pDest, vec, _MM_DOWNC_NONE, _MM_SUBSET32_1, _MM_HINT_NONE );
}

/// Store 16 bytes of data from a SIMD vector into memory.
///
/// For platforms with only 16-byte SIMD vectors, this has the same effect as StoreAligned().
///
/// @param[in] pDest  Address in which to store the data (must be aligned to a 16-byte boundary).
/// @param[in] vec    Vector containing the data to store.
///
/// @see LoadSplat128(), Store32()
void Helium::Simd::Store128( void* pDest, Helium::Simd::Register vec )
{
    _mm512_stored( pDest, vec, _MM_DOWNC_NONE, _MM_SUBSET32_4, _MM_HINT_NONE );
}

/// Fill a SIMD vector with a single-precision floating-point value splat across all vector components.
///
/// @param[in] value  Value to splat.
///
/// @return  SIMD vector containing the splat value.
Helium::Simd::Register Helium::Simd::SetSplatF32( float32_t value )
{
    return _mm512_set_1to16_ps( value );
}

/// Fill a SIMD vector with a 32-bit unsigned integer value splat across all vector components.
///
/// @param[in] value  Value to splat.
///
/// @return  SIMD vector containing the splat value.
Helium::Simd::Register Helium::Simd::SetSplatU32( uint32_t value )
{
    Helium::_Lrbni512Pun vecPun;
    vecPun.i = _mm512_set_1to16_pi( static_cast< int >( value ) );

    return vecPun.f;
}

/// Load a vector containing all zeros.
///
/// @return  Vector containing all zeros.
Helium::Simd::Register Helium::Simd::LoadZeros()
{
    return _mm512_setzero_ps();
}

/// Perform a component-wise addition of two SIMD vectors of single-precision floating-point values.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector to add.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::AddF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_add_ps( vec0, vec1 );
}

/// Perform a component-wise subtraction of one SIMD vector of single-precision floating-point values from another.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector to subtract.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::SubtractF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_sub_ps( vec0, vec1 );
}

/// Perform a component-wise multiplication of two SIMD vectors of single-precision floating-point values.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector by which to multiply.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::MultiplyF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_mul_ps( vec0, vec1 );
}

/// Perform a component-wise division of one SIMD vector of single-precision floating-point values by another.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector by which to divide.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::DivideF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_div_ps( vec0, vec1 );
}

/// Perform a component-wise multiplication of two SIMD vectors of single-precision floating-point values, and add
/// the resulting value with those in a third vector.
///
/// @param[in] vecMul0  SIMD vector.
/// @param[in] vecMul1  SIMD vector by which to mulitiply.
/// @param[in] vecAdd   SIMD vector to add to the result.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::MultiplyAddF32(
    Helium::Simd::Register vecMul0,
    Helium::Simd::Register vecMul1,
    Helium::Simd::Register vecAdd )
{
    return _mm512_madd213_ps( vecMul0, vecMul1, vecAdd );
}

/// Compute the square root of each component in a SIMD vector of single-precision floating-point values.
///
/// Note that this may be only an approximation on certain platforms, so its precision is not guaranteed to be the
/// same as using the C-library sqrtf() function on each component.
///
/// @param[in] vec  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::SqrtF32( Helium::Simd::Register vec )
{
    return _mm512_sqrt_ps( vec );
}

/// Compute the multiplicative inverse of each component in a SIMD vector of single-precision floating-point values.
///
/// Note that this may be only an approximation on certain platforms, so its precision is not guaranteed to be the
/// same as actually computing the reciprocal of each component using scalar division.
///
/// @param[in] vec  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::InverseF32( Helium::Simd::Register vec )
{
    return _mm512_recip_ps( vec );
}

/// Compute the multiplicative inverse of the square root of each component in a SIMD vector of single-precision
/// floating-point values.
///
/// Note that this may be only an approximation on certain platforms, so its precision is not guaranteed to be the
/// same as actually computing the reciprocal of the square root of each component using the C-library sqrtf()
/// function and scalar division.
///
/// @param[in] vec  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::InverseSqrtF32( Helium::Simd::Register vec )
{
    return _mm512_rsqrt_ps( vec );
}

/// Create a SIMD vector of single-precision floating-point values containing the minimum between each component in
/// the two given SIMD vectors.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::MinF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_min_ps( vec0, vec1 );
}

/// Create a SIMD vector of single-precision floating-point values containing the maximum between each component in
/// the two given SIMD vectors.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::MaxF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_max_ps( vec0, vec1 );
}

/// Compare each component in two SIMD vectors of single-precision floating-point values for equality, setting each
/// component in the result mask based on the result of the comparison.
///
/// If the corresponding components in the two given vectors are equal, the corresponding component in the result
/// mask will be set, otherwise it will be cleared.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  Mask with the result of the operation.
Helium::Simd::Mask Helium::Simd::EqualsF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_cmpeq_ps( vec0, vec1 );
}

/// Compare each component in two SIMD vectors of single-precision floating-point values for whether the component
/// in the first vector is less than the corresponding component in the second, setting each component in the result
/// mask based on the result of the comparison.
///
/// If a component in the first vector is less than the corresponding component in the second vector, the
/// corresponding component in the result mask will be set, otherwise it will be cleared.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  Mask with the result of the operation.
Helium::Simd::Mask Helium::Simd::LessF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_cmplt_ps( vec0, vec1 );
}

/// Compare each component in two SIMD vectors of single-precision floating-point values for whether the component
/// in the first vector is greater than the corresponding component in the second, setting each component in the
/// result mask based on the result of the comparison.
///
/// If a component in the first vector is greater than the corresponding component in the second vector, the
/// corresponding component in the result mask will be set, otherwise it will be cleared.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  Mask with the result of the operation.
Helium::Simd::Mask Helium::Simd::GreaterF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_cmple_ps( vec1, vec0 );
}

/// Compare each component in two SIMD vectors of single-precision floating-point values for whether the component
/// in the first vector is less than or equal to the corresponding component in the second, setting each component
/// in the result mask based on the result of the comparison.
///
/// If a component in the first vector is less than or equal to the corresponding component in the second vector,
/// the corresponding component in the result mask will be set, otherwise it will be cleared.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  Mask with the result of the operation.
Helium::Simd::Mask Helium::Simd::LessEqualsF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_cmple_ps( vec0, vec1 );
}

/// Compare each component in two SIMD vectors of single-precision floating-point values for whether the component
/// in the first vector is greater than or equal to the corresponding component in the second, setting each
/// component in the result mask based on the result of the comparison.
///
/// If a component in the first vector is greater than or equal to the corresponding component in the second vector,
/// the corresponding component in the result mask will be set, otherwise it will be cleared.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  Mask with the result of the operation.
Helium::Simd::Mask Helium::Simd::GreaterEqualsF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm512_cmplt_ps( vec1, vec0 );
}

/// Compute the bitwise-AND of two SIMD vectors.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::And( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    Helium::_Lrbni512Pun vec0Pun, vec1Pun, resultPun;
    vec0Pun.f = vec0;
    vec1Pun.f = vec1;

    resultPun.i = _mm512_and_pi( vec0Pun.i, vec1Pun.i );

    return resultPun.f;
}

/// Compute the bitwise-AND of the one's complement (bitwise-NOT) of a vector with another vector.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation (that is, the bitwise-AND of the second vector and the
///          complement of the first vector).
Helium::Simd::Register Helium::Simd::AndNot( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    Helium::_Lrbni512Pun vec0Pun, vec1Pun, resultPun;
    vec0Pun.f = vec0;
    vec1Pun.f = vec1;

    resultPun.i = _mm512_andn_pi( vec0Pun.i, vec1Pun.i );

    return resultPun.f;
}

/// Compute the bitwise-OR of two SIMD vectors.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::Or( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    Helium::_Lrbni512Pun vec0Pun, vec1Pun, resultPun;
    vec0Pun.f = vec0;
    vec1Pun.f = vec1;

    resultPun.i = _mm512_or_pi( vec0Pun.i, vec1Pun.i );

    return resultPun.f;
}

/// Compute the bitwise-XOR of two SIMD vectors.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::Xor( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    Helium::_Lrbni512Pun vec0Pun, vec1Pun, resultPun;
    vec0Pun.f = vec0;
    vec1Pun.f = vec1;

    resultPun.i = _mm512_xor_pi( vec0Pun.i, vec1Pun.i );

    return resultPun.f;
}

#endif  // HELIUM_SIMD_LRBNI
